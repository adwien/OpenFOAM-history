package FoamXServer.CaseServer;


/**
* FoamXServer/CaseServer/IPatchPhysicalTypeDescriptorHelper.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from FoamX.idl
* 02 April 2007 09:53:40 o'clock BST
*/


// ---------------------------------------------------------------------
abstract public class IPatchPhysicalTypeDescriptorHelper
{
  private static String  _id = "IDL:FoamXServer/CaseServer/IPatchPhysicalTypeDescriptor:1.0";

  public static void insert (org.omg.CORBA.Any a, FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor that)
  {
    org.omg.CORBA.portable.OutputStream out = a.create_output_stream ();
    a.type (type ());
    write (out, that);
    a.read_value (out.create_input_stream (), type ());
  }

  public static FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor extract (org.omg.CORBA.Any a)
  {
    return read (a.create_input_stream ());
  }

  private static org.omg.CORBA.TypeCode __typeCode = null;
  synchronized public static org.omg.CORBA.TypeCode type ()
  {
    if (__typeCode == null)
    {
      __typeCode = org.omg.CORBA.ORB.init ().create_interface_tc (FoamXServer.CaseServer.IPatchPhysicalTypeDescriptorHelper.id (), "IPatchPhysicalTypeDescriptor");
    }
    return __typeCode;
  }

  public static String id ()
  {
    return _id;
  }

  public static FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor read (org.omg.CORBA.portable.InputStream istream)
  {
    return narrow (istream.read_Object (_IPatchPhysicalTypeDescriptorStub.class));
  }

  public static void write (org.omg.CORBA.portable.OutputStream ostream, FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor value)
  {
    ostream.write_Object ((org.omg.CORBA.Object) value);
  }

  public static FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor narrow (org.omg.CORBA.Object obj)
  {
    if (obj == null)
      return null;
    else if (obj instanceof FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor)
      return (FoamXServer.CaseServer.IPatchPhysicalTypeDescriptor)obj;
    else if (!obj._is_a (id ()))
      throw new org.omg.CORBA.BAD_PARAM ();
    else
    {
      org.omg.CORBA.portable.Delegate delegate = ((org.omg.CORBA.portable.ObjectImpl)obj)._get_delegate ();
      FoamXServer.CaseServer._IPatchPhysicalTypeDescriptorStub stub = new FoamXServer.CaseServer._IPatchPhysicalTypeDescriptorStub ();
      stub._set_delegate(delegate);
      return stub;
    }
  }

}
