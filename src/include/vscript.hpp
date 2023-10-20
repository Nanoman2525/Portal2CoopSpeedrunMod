#ifndef VSCRIPT_HPP
#define VSCRIPT_HPP

// shims!
typedef int ScriptLanguage_t;
typedef void* HSCRIPT;
typedef int ScriptStatus_t;
typedef void ScriptVariant_t;
typedef void ScriptFunctionBinding_t;
typedef void ScriptClassDesc_t;
typedef void CUtlBuffer;
typedef void* ScriptOutputFunc_t;
typedef void* ScriptErrorFunc_t;

class IScriptVM {
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual bool ConnectDebugger() = 0;
    virtual void DisconnectDebugger() = 0;
    virtual ScriptLanguage_t GetLanguage() = 0;
    virtual const char* GetLanguageName() = 0;
    virtual void AddSearchPath(const char* pszSearchPath) = 0;
    virtual bool Frame(float simTime) = 0;
    virtual ScriptStatus_t Run(const char* pszScript, bool bWait = true) = 0;
    virtual HSCRIPT CompileScript(const char* pszScript, const char* pszId = nullptr) = 0;
    virtual void ReleaseScript(HSCRIPT) = 0;
    virtual ScriptStatus_t Run(HSCRIPT hScript, HSCRIPT hScope = nullptr, bool bWait = true) = 0;
    virtual ScriptStatus_t Run(HSCRIPT hScript, bool bWait) = 0;
    virtual HSCRIPT CreateScope(const char* pszScope, HSCRIPT hParent = nullptr) = 0;
    virtual void ReleaseScope(HSCRIPT hScript) = 0;
    virtual HSCRIPT LookupFunction(const char* pszFunction, HSCRIPT hScope = nullptr) = 0;
    virtual void ReleaseFunction(HSCRIPT hScript) = 0;
    virtual ScriptStatus_t ExecuteFunction(HSCRIPT hFunction, ScriptVariant_t* pArgs, int nArgs, ScriptVariant_t* pReturn, HSCRIPT hScope, bool bWait) = 0;
    virtual void RegisterFunction(ScriptFunctionBinding_t* pScriptFunction) = 0;
    virtual bool RegisterClass(ScriptClassDesc_t* pClassDesc) = 0;
    virtual HSCRIPT RegisterInstance(ScriptClassDesc_t* pDesc, void* pInstance) = 0;
    virtual void SetInstanceUniqeId(HSCRIPT hInstance, const char* pszId) = 0;
    virtual void RemoveInstance(HSCRIPT) = 0;
    virtual void* GetInstanceValue(HSCRIPT hInstance, ScriptClassDesc_t* pExpectedType = nullptr) = 0;
    virtual bool GenerateUniqueKey(const char* pszRoot, char* pBuf, int nBufSize) = 0;
    virtual bool ValueExists(HSCRIPT hScope, const char* pszKey) = 0;
    virtual bool SetValue(HSCRIPT hScope, const char* pszKey, const char* pszValue) = 0;
    virtual bool SetValue(HSCRIPT hScope, const char* pszKey, const ScriptVariant_t* value) = 0;
    virtual void CreateTable(ScriptVariant_t* Table) = 0;
    virtual int    GetNumTableEntries(HSCRIPT hScope) = 0;
    virtual int GetKeyValue(HSCRIPT hScope, int nIterator, ScriptVariant_t* pKey, ScriptVariant_t* pValue) = 0;
    virtual bool GetValue(HSCRIPT hScope, const char* pszKey, ScriptVariant_t* pValue) = 0;
    virtual void ReleaseValue(ScriptVariant_t* value) = 0;
    virtual bool ClearValue(HSCRIPT hScope, const char* pszKey) = 0;
    virtual void WriteState(CUtlBuffer* pBuffer) = 0;
    virtual void ReadState(CUtlBuffer* pBuffer) = 0;
    virtual void RemoveOrphanInstances() = 0;
    virtual void DumpState() = 0;
    virtual void SetOutputCallback(ScriptOutputFunc_t pFunc) = 0;
    virtual void SetErrorCallback(ScriptErrorFunc_t pFunc) = 0;
    virtual bool RaiseException(const char* pszExceptionText) = 0;
};

#endif // VSCRIPT_HPP
