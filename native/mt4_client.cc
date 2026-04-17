#include <filesystem>

#include "mt4_client.h"
#include "utils/mt4_errors.h"
#include "utils/mt4_log.h"

MT4Client::MT4Client(const std::string &dllPath) : dllPath_(dllPath)
{
  ValidateDllPath();
  LoadApi();
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
    factory_ = new CManagerFactory(dllPath_.c_str());
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
  if (dllPath_.empty())
  {
    throw std::runtime_error("MT4 DLL path is empty");
  }

  if (!std::filesystem::exists(dllPath_))
  {
    throw std::runtime_error("MT4 DLL path does not exist");
  }

  if (!std::filesystem::is_regular_file(dllPath_))
  {
    throw std::runtime_error("MT4 DLL path is not a file");
  }
}
