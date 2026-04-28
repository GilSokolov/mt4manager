#include <filesystem>

#include "mt4_client.h"

#include "utils/mt4_errors.h"
#include "utils/mt4_log.h"
#include "utils/mt4_logger.h"

int BuildPumpFlags(const PumpingOptions &options)
{
  int flags = 0;

  if (!options.ticks)
    flags |= CLIENT_FLAGS_HIDETICKS;
  if (!options.news)
    flags |= CLIENT_FLAGS_HIDENEWS;
  if (!options.mail)
    flags |= CLIENT_FLAGS_HIDEMAIL;
  if (!options.onlineUsers)
    flags |= CLIENT_FLAGS_HIDEONLINE;
  if (!options.users)
    flags |= CLIENT_FLAGS_HIDEUSERS;

  if (options.fullNews)
    flags |= CLIENT_FLAGS_SENDFULLNEWS;

  return flags;
}

MT4Client::MT4Client(MT4ClientConfig config)
    : config_(std::move(config))
{
  ValidateDllPath();
  LoadApi();
  MT4Logger::Instance().SetLevel(config_.logLevel);
}

MT4Client::~MT4Client()
{
  Close();
}

void MT4Client::LoadApi()
{
  MT4_INFO_LOG("LoadApi start");

  if (manager_)
  {
    MT4_DEBUG_LOG("LoadApi skipped (already initialized)");
    return;
  }

  try
  {
    factory_ = new CManagerFactory(config_.dllPath.c_str());
    MT4_DEBUG_LOG("factory constructed");

    const int winsockCode = factory_->WinsockStartup();
    if (winsockCode != RET_OK)
    {
      MT4_ERROR_LOG("WinsockStartup failed");
      throw std::runtime_error("CManagerFactory::WinsockStartup failed");
    }
    MT4_DEBUG_LOG("WinsockStartup done");
    winsockStarted_ = true;

    if (!factory_->IsValid())
    {
      MT4_ERROR_LOG("CManagerFactory is not valid");
      throw std::runtime_error("CManagerFactory is not valid");
    }

    manager_ = factory_->Create(ManAPIVersion);
    MT4_DEBUG_LOG("Create returned manager=" << (manager_ ? "yes" : "no"));

    if (!manager_)
    {
      MT4_ERROR_LOG("CManagerFactory::Create failed");
      throw std::runtime_error("CManagerFactory::Create failed");
    }
  }

  catch (...)
  {
    MT4_ERROR_LOG("LoadApi failed, cleaning up");
    UnloadApi();
    throw;
  }

  MT4_INFO_LOG("LoadApi done");
}

void MT4Client::UnloadApi() noexcept
{
  MT4_INFO_LOG("UnloadApi start");

  if (manager_)
  {
    const int rc = manager_->Release();
    if (rc != RET_OK)
    {
      MT4_ERROR_LOG("Release failed with code " << rc);
    }
    else
    {
      MT4_DEBUG_LOG("manager released");
    }

    manager_ = nullptr;
  }

  if (factory_)
  {
    if (winsockStarted_)
    {
      factory_->WinsockCleanup();
      winsockStarted_ = false;
      MT4_DEBUG_LOG("WinsockCleanup done");
    }

    delete factory_;
    factory_ = nullptr;
    MT4_DEBUG_LOG("factory deleted");
  }

  connected_ = false;
  loggedIn_ = false;

  MT4_INFO_LOG("UnloadApi done");
}

void MT4Client::EnsureOpen() const
{
  if (closed_.load())
  {
    throw std::runtime_error("MT4 client is already closed");
  }
}

void MT4Client::EnsureManager() const
{
  if (!manager_)
  {
    throw std::runtime_error("MT4 manager is not initialized");
  }
}

void MT4Client::Connect(const std::string &server)
{
  std::lock_guard<std::mutex> lock(mutex_);

  EnsureOpen();
  EnsureManager();

  if (connected_)
  {
    MT4_DEBUG_LOG("Connect skipped (already connected)");
    return;
  }

  MT4_INFO_LOG("Connect start");

  const int rc = manager_->Connect(server.c_str());

  ThrowMt4Error("Connect", rc, manager_);

  connected_ = true;

  MT4_INFO_LOG("Connect done");
}

void MT4Client::Login(int login, const std::string &password)
{
  std::lock_guard<std::mutex> lock(mutex_);

  EnsureOpen();
  EnsureManager();

  if (!connected_)
  {
    MT4_ERROR_LOG("Cannot login before connect");
    throw std::runtime_error("Cannot login before connect");
  }

  if (loggedIn_)
  {
    MT4_DEBUG_LOG("Login skipped (already logged in)");
    return;
  }

  MT4_INFO_LOG("Login start (login=" << login << ")");

  const int rc = manager_->Login(login, password.c_str());
  ThrowMt4Error("Login", rc, manager_);

  loggedIn_ = true;
  MT4_INFO_LOG("Login success");
}

void MT4Client::Disconnect()
{
  std::lock_guard<std::mutex> lock(mutex_);

  if (closed_.load() || !manager_ || !connected_)
  {
    MT4_DEBUG_LOG("Disconnect skipped (already disconnected)");
    return;
  }

  StopPumping();

  MT4_INFO_LOG("Disconnect start");

  const int rc = manager_->Disconnect();
  ThrowMt4Error("Disconnect", rc, manager_);

  connected_ = false;
  loggedIn_ = false;
  MT4_INFO_LOG("Disconnect success");
}

void MT4Client::Close()
{
  std::lock_guard<std::mutex> lock(mutex_);

  if (closed_.exchange(true))
  {
    MT4_DEBUG_LOG("Close skipped (already closed)");
    return;
  }

  StopPumping();

  MT4_INFO_LOG("Close called");

  if (manager_ && connected_)
  {
    const int rc = manager_->Disconnect();
    if (rc != RET_OK)
    {
      MT4_ERROR_LOG("Disconnect failed with code " << rc);
    }
    else
    {
      MT4_DEBUG_LOG("Disconnected");
    }
  }

  UnloadApi();
}

void MT4Client::ValidateDllPath() const
{
  if (config_.dllPath.empty())
  {
    throw std::runtime_error("MT4 DLL path is empty");
  }

  if (!std::filesystem::exists(config_.dllPath))
  {
    throw std::runtime_error("MT4 DLL path does not exist");
  }

  if (!std::filesystem::is_regular_file(config_.dllPath))
  {
    throw std::runtime_error("MT4 DLL path is not a file");
  }
}

void MT4Client::StartPumping(const PumpingOptions &options)
{
  EnsureManager();

  if (pumping_)
  {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(pump_ready_mutex_);
    pump_started_ = false;
  }

  const int flags = BuildPumpFlags(options);

  MT4_INFO_LOG("StartPumping begin flags=" << flags);

  TogglePumping(flags);

  pump_flags_ = flags;

  WaitForPumpStart();

  pumping_ = true;

  MT4_INFO_LOG("StartPumping ready");
}

void MT4Client::StopPumping()
{
  if (!manager_ || !pumping_)
  {
    return;
  }

  MT4_INFO_LOG("StopPumping request");

  TogglePumping(pump_flags_);

  pumping_ = false;

  {
    std::lock_guard<std::mutex> lock(pump_ready_mutex_);
    pump_started_ = false;
  }

  MT4_INFO_LOG("StopPumping done");
}

void MT4Client::TogglePumping(int flags)
{
  const int code = manager_->PumpingSwitchEx(&PumpCallback, flags, this);
  ThrowMt4Error("PumpingSwitchEx", code, manager_);
}

bool MT4Client::IsPumping() const
{
  return pumping_;
}

void __stdcall MT4Client::PumpCallback(int code, int type, void *data, void *param)
{

  auto *client = static_cast<MT4Client *>(param);
  if (client)
  {
    client->NotifyPumpListeners(code, type, data);
  }
}

void MT4Client::NotifyPumpListeners(int code, int type, void *data)
{
  if (code == PUMP_START_PUMPING)
  {
    MT4_INFO_LOG("Pump event: PUMP_START_PUMPING");
    MarkPumpStarted();
  }

  if (code == PUMP_STOP_PUMPING)
  {
    MT4_INFO_LOG("Pump event: PUMP_STOP_PUMPING");
    pumping_ = false;

    std::lock_guard<std::mutex> lock(pump_ready_mutex_);
    pump_started_ = false;
  }

  auto listeners = CopyPumpListeners();
  for (const auto &listener : listeners)
  {
    if (listener)
    {
      listener(code, type, data);
    }
  }
}

int MT4Client::AddPumpListener(PumpListener listener)
{
  std::lock_guard<std::mutex> lock(pump_listeners_mutex_);

  pump_listeners_.push_back(std::move(listener));
  return static_cast<int>(pump_listeners_.size() - 1);
}

void MT4Client::RemovePumpListener(int id)
{
  std::lock_guard<std::mutex> lock(pump_listeners_mutex_);

  if (id < 0 || id >= static_cast<int>(pump_listeners_.size()))
  {
    return;
  }

  pump_listeners_[id] = PumpListener{};
}

std::vector<MT4Client::PumpListener> MT4Client::CopyPumpListeners() const
{
  std::lock_guard<std::mutex> lock(pump_listeners_mutex_);
  return pump_listeners_;
}

void MT4Client::MarkPumpStarted()
{
  {
    std::lock_guard<std::mutex> lock(pump_ready_mutex_);
    pump_started_ = true;
  }

  pump_ready_cv_.notify_all();
}

void MT4Client::WaitForPumpStart()
{
  std::unique_lock<std::mutex> lock(pump_ready_mutex_);

  bool ready = pump_ready_cv_.wait_for(
      lock,
      std::chrono::seconds(10),
      [this]
      {
        return pump_started_;
      });

  if (!ready)
  {
    throw std::runtime_error("Timed out waiting for MT4 pumping to start");
  }
}

void MT4Client::EnsureNotPumping() const
{
  if (pumping_)
  {
    throw std::runtime_error(
        "This MT4Manager instance is in pumping mode. Use a separate MT4Manager instance for commands.");
  }
}