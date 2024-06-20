// Registry.cpp

#include "Registry.h"

DWORD RegistryGetValue( HKEY hKeyTopLevel, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwDefaultValue )
{
	DWORD dwResult;

	HKEY hKeySub;

	// Open registry sub-key
	if( RegOpenKeyEx( hKeyTopLevel, lpszSubKey, 0, KEY_READ, &hKeySub ) == ERROR_SUCCESS )
	{
		// Successfully opened registry sub-key
		DWORD dwType;
		DWORD dwSize = sizeof( dwResult );

		// Query value
		if( RegQueryValueEx( hKeySub, lpszValueName, 0, &dwType, ( LPBYTE )&dwResult, &dwSize ) != ERROR_SUCCESS )
		{
			// Unable to query value

			// Use default value
			dwResult = dwDefaultValue;

		} // End of unable to query value

		// Close registry sub-key
		RegCloseKey( hKeySub );

	} // End of successfully opened registry sub-key
	else
	{
		// Unable to open registry sub-key

		// Use default value
		dwResult = dwDefaultValue;

	} // End of unable to open registry sub-key

	return dwResult;

} // End of function RegistryGetValue

BOOL RegistrySetValue( HKEY hKeyTopLevel, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwValue )
{
	BOOL bResult = FALSE;

	HKEY hKeySub;

	// Create (or open) registry sub-key
	if( RegCreateKeyEx( hKeyTopLevel, lpszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeySub, NULL ) == ERROR_SUCCESS )
	{
		// Successfully opened registry sub-key
		DWORD dwSize = sizeof( dwValue );

		// Query value
		if( RegSetValueEx ( hKeySub, lpszValueName, 0, REG_DWORD, ( LPBYTE )&dwValue, dwSize ) == ERROR_SUCCESS )
		{
			// Successfully queried value

			// Update result
			bResult = TRUE;

		} // End of successfully queried value

		// Close registry sub-key
		RegCloseKey( hKeySub );

	} // End of successfully opened registry sub-key

	return bResult;

} // End of function RegistrySetValue