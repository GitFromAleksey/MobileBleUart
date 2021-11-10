'[startrename_merged_output_hex.vbs]
Dim Args
Set Args = WScript.Arguments

if (Args.Count < 3) then
  MsgBox "Error generate version file."
    
else
  'get fw version out of autover.ini
  verfile_h   = Args(0)
  tarfile     = Args(1)
  output_path = Args(2)


  Dim FSO, FileOutStream, ver, name_buff
  Dim ver_str, time_str, date_str
  Dim ver_arr
  Set FSO = CreateObject("Scripting.FileSystemObject")
  if FSO.FileExists(verfile_h) then
    Set FileOutStream = FSO.OpenTextFile(verfile_h)
    ver_str = FSO.OpenTextFile(verfile_h).ReadAll
    ver_arr = Split(ver_str)
    ver = CInt(ver_arr(2))
    FileOutStream.Close
  else
    ver = 1
  end if
  
  date_str  = cstr(Year(Now()))+"-"+cstr(Month(Now()))+"-"+cstr(Day(Now()))
  time_str  = cstr(Hour(Now()))+"-"+cstr(Minute(Now()))


  name_buff = "BRC Firmware v0x0" + hex(ver) + " Release.hex"
  if FSO.FileExists(output_path & name_buff) then
    FSO.DeleteFile(output_path & name_buff)
  end if

  if FSO.FileExists(tarfile) then
    FSO.CopyFile tarfile, output_path & name_buff
  end if
end if
'[end ofrename_merged_output_hex.vbs]