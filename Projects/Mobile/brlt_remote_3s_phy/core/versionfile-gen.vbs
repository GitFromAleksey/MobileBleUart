'[Начало versionfile-gen.vbs]
Dim Args
Set Args = WScript.Arguments

if (Args.Count < 2) then
  MsgBox "Error generate version file."
else
  'получаем имя выходного файла
  verfile_h = Args(0)
  output_hex = Args(1)

  Dim FSO, FileOutStream, ver 
  Dim ver_str
  Dim ver_arr
  Set FSO = CreateObject("Scripting.FileSystemObject")
  if FSO.FileExists(verfile_h) then
    Set FileOutStream = FSO.OpenTextFile(verfile_h)
    ver_str = FSO.OpenTextFile(verfile_h).ReadAll
    ver_arr = Split(ver_str)
    ver = CInt(ver_arr(2))
    ver = ver + 1
    FileOutStream.Close
  else
    ver = 1
  end if

  Set FileOutStream = FSO.OpenTextFile(verfile_h, 2, true, 0)
  FileOutStream.Write("#define SW_VERSION "&CStr(ver))
  FileOutStream.Close

  if FSO.FileExists(output_hex) then
    FSO.DeleteFile(output_hex)
  end if
end if
'[Конец versionfile-gen.vbs]