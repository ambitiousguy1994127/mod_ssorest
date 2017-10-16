#include <httpd.h>
#include <http_core.h>
#include <http_protocol.h>
#include "SSORestPlugin.h"
#include "SSORestPluginPool.h"
#include "Logger.h"
#include <fstream>
using namespace ssorest;

static const char* setEnable(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setTraceEnable(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setUseServerNameAsDefault(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setSendFormParameters(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setAccountName(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setGatewayUrl(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setLocalRootPath(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setPluginId(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setSecretKey(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setSSOZone(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setIgnoreExt(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setIgnoreUrl(cmd_parms* command, void* /*config*/, const char* argument);
static void registerHooks(apr_pool_t* );
static int pluginInitialize(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);


static const command_rec moduleDirectives[] =
{
    AP_INIT_TAKE1
    (
        "SSORestEnabled",            
        reinterpret_cast<cmd_func>(setEnable), 
        NULL, 
        OR_ALL, 
        "Enable or disable mod_ssorest"
    ),

    AP_INIT_TAKE1
    (
        "SSORestTrace",
        reinterpret_cast<cmd_func>(setTraceEnable), 
        NULL, 
        OR_ALL, 
        "Enable or disable libcurl debug"
    ),

    AP_INIT_TAKE1
    (
        "SSORestUseServerNameAsDefault",
        reinterpret_cast<cmd_func>(setUseServerNameAsDefault), 
        NULL, 
        OR_ALL, 
        ""
    ),

    AP_INIT_TAKE1
    (
        "SSORestSendFormParameters",
        reinterpret_cast<cmd_func>(setSendFormParameters), 
        NULL, 
        OR_ALL, 
        ""
    ),

    AP_INIT_TAKE1
    (
        "SSORestACOName",        
        reinterpret_cast<cmd_func>(setAccountName), 
        NULL, 
        OR_ALL, 
        ""
    ),
          
    AP_INIT_TAKE1
    (
        "SSORestGatewayUrl",
        reinterpret_cast<cmd_func>(setGatewayUrl), 
        NULL, 
        OR_ALL, 
        "Gateway Location"
    ),

    AP_INIT_TAKE1
    (
        "SSORestLocalContent",
        reinterpret_cast<cmd_func>(setLocalRootPath), 
        NULL, 
        OR_ALL, 
        "Gateway Location"
    ),

    AP_INIT_TAKE1
    (
        "SSORestPluginId",
        reinterpret_cast<cmd_func>(setPluginId), 
        NULL, 
        OR_ALL, 
        ""
    ),

    AP_INIT_TAKE1
    (
        "SSORestSecretKey",
        reinterpret_cast<cmd_func>(setSecretKey), 
        NULL, 
        OR_ALL,
        ""
    ),

    AP_INIT_ITERATE
    (
        "SSORestSSOZone",
        reinterpret_cast<cmd_func>(setSSOZone), 
        NULL, 
        OR_ALL, 
        ""
    ),
    
    AP_INIT_ITERATE
    (
        "SSORestIgnoreExt",
        reinterpret_cast<cmd_func>(setIgnoreExt), 
        NULL, 
        OR_ALL, 
        ""
    ),

    AP_INIT_ITERATE
    (
        "SSORestIgnoreUrl",
        reinterpret_cast<cmd_func>(setIgnoreUrl), 
        NULL, 
        OR_ALL, 
        ""
    ),

    { NULL }
};

extern "C"
{
    AP_DECLARE_MODULE(ssorest) = {
        STANDARD20_MODULE_STUFF,
        NULL,                   /* Per-directory configuration handler */
        NULL,                   /* Merge handler for per-directory configurations */
        NULL,                   /* Per-server configuration handler */
        NULL,                   /* Merge handler for per-server configurations */
        moduleDirectives, 
        registerHooks 
    };
}

namespace ssorest
{
    int getApacheModuleIndex()
    {
        return APLOG_MODULE_INDEX;
    }
    static std::shared_ptr<SSORestPlugin> getSSORestPluginFrom(server_rec* server)
    {
        static SSORestPluginPool ssorestpluginPool;
        auto rawVirtualServerName = server->server_hostname;
        if (!rawVirtualServerName)
            throw std::logic_error("ServerName is not set, check virtual server settings");
        std::string virtualServerName(rawVirtualServerName);
        return ssorestpluginPool[virtualServerName];
    }
}

static const char* setEnable(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if (::strcasecmp(argument, "on") == 0)
        ssorestplugin->setEnable(true);
    else
        ssorestplugin->setEnable(false);
    return NULL;
}
static const char* setTraceEnable(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if (::strcasecmp(argument, "on") == 0)
        ssorestplugin->setTraceEnable(true);
    else
        ssorestplugin->setTraceEnable(false);
    return NULL;
}
static const char* setUseServerNameAsDefault(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if (::strcasecmp(argument, "on") == 0)
        ssorestplugin->setUseServerNameAsDefault(true);
    else
        ssorestplugin->setUseServerNameAsDefault(false);
    return NULL;
}
static const char* setSendFormParameters(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if (::strcasecmp(argument, "on") == 0)
        ssorestplugin->setSendFormParameters(true);
    else
        ssorestplugin->setSendFormParameters(false);
    return NULL;
}
static const char* setAccountName(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->setAccountName(argument);
    return NULL;
}
static const char* setGatewayUrl(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->setGatewayUrl(argument);
    return NULL;
}
static const char* setLocalRootPath(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->setLocalRootPath(argument);
    return NULL;
}
static const char* setPluginId(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->setPluginId(argument);
    return NULL;
}
static const char* setSecretKey(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->setSecretKey(argument);
    return NULL;
}
static const char* setSSOZone(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    ssorestplugin->addSSOZone(argument);
    return NULL;
}

static const char* setIgnoreExt(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if(*argument != '.')
        return "SSORestIgnoreExt directive should be start with '.'";
    ssorestplugin->addIgnoreExt(argument + 1);
    return NULL;
}

static const char* setIgnoreUrl(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto ssorestplugin = getSSORestPluginFrom(command->server);
    if(*argument != '/')
        return "SSORestIgnoreUrl directive should be start with '/'";
    ssorestplugin->addIgnoreUrl(argument);
    return NULL;
}

static int processRequest(request_rec* r) {
    int httpResult = OK;
    auto ssorestplugin = getSSORestPluginFrom(r->server);
    httpResult = ssorestplugin->process(r);
    Logger::emerg(r, "Request to Gateway had result code: %d", httpResult);
    return httpResult;
}

static int pluginInitialize(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
    /* pluginInitialize() will be called twice, and if it's a DSO
     * then all static data from the first call will be lost. Only
     * set up our static data on the second call. */
    if (ap_state_query(AP_SQ_MAIN_STATE) == AP_SQ_MS_CREATE_PRE_CONFIG)
        return OK;
    
    ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s, "SSO/Rest Plugin initialized");

#if defined(SVN_REV) && defined(MOD_VER)
    ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, s, "SSO/Rest Plugin for NGINX v%s build %s", MOD_VER, SVN_REV);
#endif
    return OK;
}

static void registerHooks(apr_pool_t* ) {
    ::ap_hook_post_config(pluginInitialize, NULL, NULL, APR_HOOK_MIDDLE);
    ::ap_hook_check_access(processRequest, NULL, NULL, APR_HOOK_MIDDLE,
                      AP_AUTH_INTERNAL_PER_URI);
}
