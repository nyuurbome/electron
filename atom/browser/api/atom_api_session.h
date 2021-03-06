// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_SESSION_H_
#define ATOM_BROWSER_API_ATOM_API_SESSION_H_

#include <string>
#include <vector>

#include "atom/browser/api/trackable_object.h"
#include "atom/browser/atom_blob_reader.h"
#include "atom/browser/net/resolve_proxy_helper.h"
#include "base/values.h"
#include "content/public/browser/download_manager.h"
#include "native_mate/handle.h"
#include "net/base/completion_callback.h"

class GURL;

namespace base {
class FilePath;
}

namespace mate {
class Arguments;
class Dictionary;
}  // namespace mate

namespace net {
class ProxyConfig;
}

namespace atom {

class AtomBrowserContext;

namespace api {

class Session : public mate::TrackableObject<Session>,
                public content::DownloadManager::Observer {
 public:
  enum class CacheAction {
    CLEAR,
    STATS,
  };

  // Gets or creates Session from the |browser_context|.
  static mate::Handle<Session> CreateFrom(v8::Isolate* isolate,
                                          AtomBrowserContext* browser_context);

  // Gets the Session of |partition|.
  static mate::Handle<Session> FromPartition(
      v8::Isolate* isolate,
      const std::string& partition,
      const base::DictionaryValue& options = base::DictionaryValue());

  AtomBrowserContext* browser_context() const { return browser_context_.get(); }

  // mate::TrackableObject:
  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  // Methods.
  void ResolveProxy(const GURL& url,
                    const ResolveProxyHelper::ResolveProxyCallback& callback);
  template <CacheAction action>
  void DoCacheAction(const net::CompletionCallback& callback);
  void ClearStorageData(mate::Arguments* args);
  void FlushStorageData();
  void SetProxy(const mate::Dictionary& options, const base::Closure& callback);
  void SetDownloadPath(const base::FilePath& path);
  void EnableNetworkEmulation(const mate::Dictionary& options);
  void DisableNetworkEmulation();
  void SetCertVerifyProc(v8::Local<v8::Value> proc, mate::Arguments* args);
  void SetPermissionRequestHandler(v8::Local<v8::Value> val,
                                   mate::Arguments* args);
  void SetPermissionCheckHandler(v8::Local<v8::Value> val,
                                 mate::Arguments* args);
  void ClearHostResolverCache(mate::Arguments* args);
  void ClearAuthCache(mate::Arguments* args);
  void AllowNTLMCredentialsForDomains(const std::string& domains);
  void SetUserAgent(const std::string& user_agent, mate::Arguments* args);
  std::string GetUserAgent();
  void GetBlobData(const std::string& uuid,
                   const AtomBlobReader::CompletionCallback& callback);
  void CreateInterruptedDownload(const mate::Dictionary& options);
  void SetPreloads(const std::vector<base::FilePath::StringType>& preloads);
  std::vector<base::FilePath::StringType> GetPreloads() const;
  v8::Local<v8::Value> Cookies(v8::Isolate* isolate);
  v8::Local<v8::Value> Protocol(v8::Isolate* isolate);
  v8::Local<v8::Value> WebRequest(v8::Isolate* isolate);
  v8::Local<v8::Value> NetLog(v8::Isolate* isolate);

 protected:
  Session(v8::Isolate* isolate, AtomBrowserContext* browser_context);
  ~Session() override;

  // content::DownloadManager::Observer:
  void OnDownloadCreated(content::DownloadManager* manager,
                         download::DownloadItem* item) override;

 private:
  // Cached object.
  v8::Global<v8::Value> cookies_;
  v8::Global<v8::Value> protocol_;
  v8::Global<v8::Value> web_request_;
  v8::Global<v8::Value> net_log_;

  // The client id to enable the network throttler.
  base::UnguessableToken network_emulation_token_;

  scoped_refptr<AtomBrowserContext> browser_context_;

  DISALLOW_COPY_AND_ASSIGN(Session);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_SESSION_H_
