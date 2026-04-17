#include "mt4_client.h"

#include <utility>

#include "utils/dll_loader.h"
#include "utils/mt4_errors.h"

#include <iostream>

MT4Client::MT4Client(const std::string &dllPath) : dllPath_(dllPath)
{
  std::cerr << "[mt4] client ctor start" << std::endl;
  LoadApi();
  std::cerr << "[mt4] client ctor done" << std::endl;
}

MT4Client::~MT4Client()
{
  Close();
}

void MT4Client::LoadApi()
{
  std::cerr << "[mt4] LoadApi start" << std::endl;

  if (factory_)
  {
    return;
  }

  factory_ = new CManagerFactory(dllPath_.c_str());
  std::cerr << "[mt4] factory constructed" << std::endl;

  factory_->WinsockStartup();
  std::cerr << "[mt4] WinsockStartup done" << std::endl;

  if (!factory_->IsValid())
  {
    throw std::runtime_error("CManagerFactory is not valid");
  }

  manager_ = factory_->Create(ManAPIVersion);
  std::cerr << "[mt4] Create returned manager=" << (manager_ ? "yes" : "no") << std::endl;

  if (!manager_)
  {
    throw std::runtime_error("CManagerFactory::Create failed");
  }

  std::cerr << "[mt4] LoadApi done" << std::endl;
}

void MT4Client::UnloadApi() noexcept
{
  if (manager_)
  {
    try
    {
      manager_->Release();
    }
    catch (...)
    {
    }
    manager_ = nullptr;
  }

  if (factory_)
  {
    try
    {
      factory_->WinsockCleanup();
    }
    catch (...)
    {
    }

    delete factory_;
    factory_ = nullptr;
  }

  connected_ = false;
  loggedIn_ = false;
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
    return;
  }

  const int rc = manager_->Connect(server.c_str());
  ThrowMt4Error("Connect", rc, manager_);

  connected_ = true;
}

void MT4Client::Login(int login, const std::string &password)
{
  std::lock_guard<std::mutex> lock(mutex_);

  EnsureOpen();
  EnsureManager();

  if (!connected_)
  {
    throw std::runtime_error("Cannot login before connect");
  }

  if (loggedIn_)
  {
    return;
  }

  const int rc = manager_->Login(login, password.c_str());
  ThrowMt4Error("Login", rc, manager_);

  loggedIn_ = true;
}

void MT4Client::Disconnect()
{
  std::lock_guard<std::mutex> lock(mutex_);

  if (closed_.load() || !manager_)
  {
    return;
  }

  manager_->Disconnect();

  connected_ = false;
  loggedIn_ = false;
}

void MT4Client::Close()
{
  std::lock_guard<std::mutex> lock(mutex_);

  if (closed_.exchange(true))
  {
    return;
  }

  if (manager_ && connected_)
  {
    try
    {
      manager_->Disconnect();
    }
    catch (...)
    {
    }
  }

  UnloadApi();
}