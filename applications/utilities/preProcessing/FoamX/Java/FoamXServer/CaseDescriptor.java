package FoamXServer;


/**
* FoamXServer/CaseDescriptor.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from FoamX.idl
* 02 April 2007 09:53:40 o'clock BST
*/


// Case descriptor structure.
public final class CaseDescriptor implements org.omg.CORBA.portable.IDLEntity
{
  public String rootDir = null;

  // rootDir (after expansion)
  public String rawRootDir = null;

  // rootDir (before expansion)
  public String caseName = null;
  public String app = null;
  public int nProcs = (int)0;

  // 1 or number of subdomains in parallel
  public boolean managed = false;

  // True if this is a FoamX managed case.
  public boolean locked = false;

  // True if this case is locked by another user.
  public boolean error = false;

  public CaseDescriptor ()
  {
  } // ctor

  public CaseDescriptor (String _rootDir, String _rawRootDir, String _caseName, String _app, int _nProcs, boolean _managed, boolean _locked, boolean _error)
  {
    rootDir = _rootDir;
    rawRootDir = _rawRootDir;
    caseName = _caseName;
    app = _app;
    nProcs = _nProcs;
    managed = _managed;
    locked = _locked;
    error = _error;
  } // ctor

} // class CaseDescriptor
