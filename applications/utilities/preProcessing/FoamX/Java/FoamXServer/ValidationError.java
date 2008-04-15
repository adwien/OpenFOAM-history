package FoamXServer;


/**
* FoamXServer/ValidationError.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from FoamX.idl
* 02 April 2007 09:53:40 o'clock BST
*/

public final class ValidationError extends org.omg.CORBA.UserException
{
  public FoamXServer.ErrorCode errorCode = null;
  public String errorMessage = null;
  public String itemPath = null;

  public ValidationError ()
  {
    super(ValidationErrorHelper.id());
  } // ctor

  public ValidationError (FoamXServer.ErrorCode _errorCode, String _errorMessage, String _itemPath)
  {
    super(ValidationErrorHelper.id());
    errorCode = _errorCode;
    errorMessage = _errorMessage;
    itemPath = _itemPath;
  } // ctor


  public ValidationError (String $reason, FoamXServer.ErrorCode _errorCode, String _errorMessage, String _itemPath)
  {
    super(ValidationErrorHelper.id() + "  " + $reason);
    errorCode = _errorCode;
    errorMessage = _errorMessage;
    itemPath = _itemPath;
  } // ctor

} // class ValidationError
