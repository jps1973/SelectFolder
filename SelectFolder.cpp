// SelectFolder.cpp

#include "SelectFolder.h"

// Global variables
static HWND g_hWndTitle;
static HWND g_hWndTreeView;
static HWND g_hWndOkButton;
static HWND g_hWndCancelButton;
static LPTSTR g_lpszFolderPath;
static LPTSTR g_lpszTitle;

int TreeViewWindowAddDrives()
{
	int nResult = 0;
	
	// Allocate string memory
	LPTSTR lpszLogicalDrives = new char[ STRING_LENGTH ];

	// Get logical drive strings
	if( GetLogicalDriveStrings( STRING_LENGTH, lpszLogicalDrives ) )
	{
		// Successfully got logical drive strings
		TV_INSERTSTRUCT tvis;
		HTREEITEM htiDrive;
		SHFILEINFO shfi;

		// Get first drive
		LPTSTR lpszDrive = lpszLogicalDrives;

		// Clear tree view insert structure
		ZeroMemory( &tvis, sizeof( tvis ) );

		// Initialise tree view insert structure
		tvis.hInsertAfter			= TVI_SORT;
		tvis.item.mask				= ( TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE );

		// Delete sub items from tree view window
		TreeViewWindowDeleteAllSubItems( TVI_ROOT );

		// Loop through all drives
		while( *lpszDrive )
		{
			// Get icon index
			SHGetFileInfo( (LPCTSTR)lpszDrive, 0,&shfi, sizeof(SHFILEINFO), ( SHGFI_SYSICONINDEX | SHGFI_SMALLICON ) );

			// Update tree view insert structure for sub-folder item
			tvis.hParent				= TVI_ROOT;
			tvis.item.pszText			= ( LPTSTR )lpszDrive;
			tvis.item.iImage			= shfi.iIcon;
			tvis.item.iSelectedImage	= shfi.iIcon;

			// Add sub-folder item
			htiDrive = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_INSERTITEM, ( WPARAM )0, ( LPARAM )&tvis );

			// Ensure that sub-folder item was added
			if( htiDrive )
			{
				// Successfully added sub-folder item

				// Update tree view insert structure for dummy sub-item
				tvis.hParent				= htiDrive;
				tvis.item.pszText			= ( LPTSTR )SELECT_FOLDER_TREE_VIEW_WINDOW_DUMMY_ITEM_TEXT;

				// Add dummy sub-item
				SendMessage( g_hWndTreeView, TVM_INSERTITEM, ( WPARAM )0, ( LPARAM )&tvis );

				// Update return value
				nResult ++;

			} // End of successfully added sub-folder item

			// Get next drive
			lpszDrive += ( lstrlen( lpszDrive ) + sizeof( char ) );

		}; // End of loop through all drives

	} // End of successfully got logical drive strings

	// Free string memory
	delete [] lpszLogicalDrives;

	return nResult;

} // End of function TreeViewWindowAddDrives

int TreeViewWindowAddSubFolders( HTREEITEM htiParentFolder )
{
	int nResult = 0;

	// Allocate string memory
	LPTSTR lpszParentFolderPath = new char[ STRING_LENGTH ];

	// Get parent folder path
	if( TreeViewWindowGetItemPath( htiParentFolder, lpszParentFolderPath ) )
	{
		// Successfully got parent folder path
		WIN32_FIND_DATA wfd;
		HANDLE hFind;

		// Allocate string memory
		LPTSTR lpszFullSearchPattern = new char[ STRING_LENGTH ];

		// Copy parent folder path into full search pattern
		lstrcpy( lpszFullSearchPattern, lpszParentFolderPath );

		// Append all files filter onto full search pattern
		lstrcat( lpszFullSearchPattern, ALL_FILES_FILTER );

		// Find first item
		hFind = FindFirstFile( lpszFullSearchPattern, &wfd );

		// Ensure that first item was found
		if(hFind != INVALID_HANDLE_VALUE )
		{
			// Successfully found first item
			TV_INSERTSTRUCT tvis;
			HTREEITEM htiSubFolder;
			SHFILEINFO shfi;

			// Allocate string memory
			LPTSTR lpszSubFolderPath = new char[ STRING_LENGTH ];

			// Clear tree view insert structure
			ZeroMemory( &tvis, sizeof( tvis ) );

			// Initialise tree view insert structure
			tvis.hInsertAfter			= TVI_SORT;
			tvis.item.mask				= ( TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE );

			// Delete sub items from tree view window
			TreeViewWindowDeleteAllSubItems( htiParentFolder );

			// Loop through all items
			do
			{
				// See if current item is a folder
				if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Current item is a folder

					// Ensure that current item is not dots
					if( wfd.cFileName[ 0 ] != ASCII_FULL_STOP_CHARACTER )
					{
						// Current item is not dots

						// Copy parent folder path into sub-folder path
						lstrcpy( lpszSubFolderPath, lpszParentFolderPath );

						// Append sub-folder name onto sub-folder path
						lstrcat( lpszSubFolderPath, wfd.cFileName );

						// Get icon index
						SHGetFileInfo( (LPCTSTR)lpszSubFolderPath, 0,&shfi, sizeof(SHFILEINFO), ( SHGFI_SYSICONINDEX | SHGFI_SMALLICON ) );
	
						// Update tree view insert structure for sub-folder item
						tvis.hParent				= htiParentFolder;
						tvis.item.pszText			= ( LPTSTR )wfd.cFileName;
						tvis.item.iImage			= shfi.iIcon;
						tvis.item.iSelectedImage	= shfi.iIcon;

						// Add sub-folder item
						htiSubFolder = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_INSERTITEM, ( WPARAM )0, ( LPARAM )&tvis );

						// Ensure that sub-folder item was added
						if( htiSubFolder )
						{
							// Successfully added sub-folder item

							// Update tree view insert structure for dummy sub-item
							tvis.hParent				= htiSubFolder;
							tvis.item.pszText			= ( LPTSTR )SELECT_FOLDER_TREE_VIEW_WINDOW_DUMMY_ITEM_TEXT;

							// Add dummy sub-item
							SendMessage( g_hWndTreeView, TVM_INSERTITEM, ( WPARAM )0, ( LPARAM )&tvis );

							// Update return value
							nResult ++;

						} // End of successfully added sub-folder item

					} // End of current item is not dots

				} // End of current item is a folder

			} while( FindNextFile( hFind, &wfd ) != 0 ); // End of loop through all items

			// Close file find
			FindClose( hFind );

			// Free string memory
			delete [] lpszSubFolderPath;

		} // End of successfully found first item
		else
		{
			// Unable to find first item

			// This means that we don't have access to the folder

			// Delete sub items from tree view window
			TreeViewWindowDeleteAllSubItems( htiParentFolder );

		} // End of unable to find first item

		// Free string memory
		delete [] lpszFullSearchPattern;

	} // End of successfully got parent folder path

	// Free string memory
	delete [] lpszParentFolderPath;

	return nResult;

} // End of function TreeViewWindowAddSubFolders

int TreeViewWindowDeleteAllSubItems( HTREEITEM htiParentItem )
{
	int nResult = 0;

	HTREEITEM htiSubItem;

	// Get first sub-item
	htiSubItem = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_GETNEXTITEM, ( WPARAM )TVGN_CHILD, ( LPARAM )htiParentItem );

	// Loop through all sub-items
	while( htiSubItem )
	{
		// Delete sub-item
		if( SendMessage( g_hWndTreeView, TVM_DELETEITEM, ( WPARAM )0, ( LPARAM )htiSubItem ) )
		{
			// Successfully deleted sub-item

			// Update return value
			nResult ++;

			// Get next sub-item
			htiSubItem = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_GETNEXTITEM, ( WPARAM )TVGN_CHILD, ( LPARAM )htiParentItem );

		} // End of successfully deleted sub-item
		else
		{
			// Unable to delete sub-item

			// Force exit from loop
			htiSubItem = NULL;

		} // End of unable to delete sub-item

	}; // End of loop through all sub-items

	return nResult;

} // End of function TreeViewWindowDeleteAllSubItems

BOOL TreeViewWindowExpandItem( HTREEITEM htiItem )
{
	// Expand item
	return SendMessage( g_hWndTreeView, TVM_EXPAND, ( WPARAM )TVE_EXPAND, ( LPARAM )htiItem );

} // End of function TreeViewWindowExpandItem

BOOL TreeViewWindowGetItemPath( HTREEITEM htiItem, LPTSTR lpszItemPath )
{
	BOOL bResult = FALSE;

	TVITEM tvItem;

	// Allocate string memory
	LPTSTR lpszTemp = new char[ STRING_LENGTH ];

	// Clear tree view item structure
	ZeroMemory( &tvItem, sizeof( tvItem ) );

	// Initialise tree view item structure
	tvItem.mask			= TVIF_TEXT;
	tvItem.pszText		= lpszItemPath;
	tvItem.cchTextMax	= STRING_LENGTH;
	tvItem.hItem		= htiItem;

	// Clear item path
	lpszItemPath[ 0 ] = ( char )NULL;

	// Loop through all items
	while( tvItem.hItem )
	{
		// Copy item path onto temp string
		lstrcpy( lpszTemp, lpszItemPath );

		// Get item text into item path
		if( SendMessage( g_hWndTreeView, TVM_GETITEM, ( WPARAM )0, ( LPARAM )&tvItem ) )
		{
			// Successfully got item text into item path

			// Ensure thet item path ends with a back-slash
			if( lpszItemPath[ lstrlen( lpszItemPath ) - sizeof( char ) ] != ASCII_BACK_SLASH_CHARACTER )
			{
				// Item path does not end with a back-slash

				// Append a back-slash onto item path
				lstrcat( lpszItemPath, ASCII_BACK_SLASH_STRING );

			} // End of item path does not end with a back-slash

			// Append temp string onto item path
			lstrcat( lpszItemPath, lpszTemp );

			// Update return value
			bResult = TRUE;

			// Get parent tree item
			tvItem.hItem = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_GETNEXTITEM, ( WPARAM )TVGN_PARENT, ( LPARAM )tvItem.hItem );

		} // End of successfully got item text into item path
		else
		{
			// Unable to get item text into item path

			// Force exit from loop
			tvItem.hItem = NULL;

		} // End of unable to get item text into item path

	}; // End of loop through all items

	// Free string memory
	delete [] lpszTemp;

	return bResult;

} // End of function TreeViewWindowGetItemPath

BOOL TreeViewWindowGetItemPath( LPTSTR lpszItemPath )
{
	BOOL bResult = FALSE;

	HTREEITEM htiSelected;

	// Get selected tree item
	htiSelected = ( HTREEITEM )SendMessage( g_hWndTreeView, TVM_GETNEXTITEM, ( WPARAM )TVGN_CARET, ( LPARAM )NULL );

	// Ensure that selected tree item was got
	if( htiSelected )
	{
		// Successfully got selected tree item

		// Get item path
		bResult = TreeViewWindowGetItemPath( htiSelected, lpszItemPath );

	} // End of successfully got selected tree item

	return bResult;

} // End of function TreeViewWindowGetItemPath

BOOL TreeViewWindowGetItemText( HTREEITEM htiItem, LPTSTR lpszItemText )
{
	BOOL bResult;

	TVITEM tvItem;

	// Clear tree view item structure
	ZeroMemory( &tvItem, sizeof( tvItem ) );

	// Initialise tree view item structure
	tvItem.mask			= TVIF_TEXT;
	tvItem.pszText		= lpszItemText;
	tvItem.cchTextMax	= STRING_LENGTH;
	tvItem.hItem		= htiItem;

	// Get item text
	bResult = SendMessage( g_hWndTreeView, TVM_GETITEM, ( WPARAM )0, ( LPARAM )&tvItem );

	return bResult;

} // End of function TreeViewWindowGetItemText

BOOL SelectFolderWindowMove( HWND hWndSelectFolder, int nLeft, int nTop )
{
	BOOL bResult = FALSE;

	int nDesktopWindowWidth;
	int nDesktopWindowHeight;
	int nMaximumLeft;
	int nMaximumTop;
	RECT rcWorkArea;

	// Get work area size
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );

	// Calculate desktop window size
	nDesktopWindowWidth		= ( rcWorkArea.right - rcWorkArea.left );
	nDesktopWindowHeight	= ( rcWorkArea.bottom - rcWorkArea.top );

	// Calculate maximum window position
	nMaximumLeft	= ( nDesktopWindowWidth - SELECT_FOLDER_WINDOW_WIDTH );
	nMaximumTop		= ( nDesktopWindowHeight - SELECT_FOLDER_WINDOW_HEIGHT );

	// Limit window position to fit on screen
	nLeft	= ( ( nLeft < 0 ) ? 0 : nLeft );
	nTop	= ( ( nTop < 0 ) ? 0 : nTop );
	nLeft	= ( ( nLeft > nMaximumLeft ) ? nMaximumLeft : nLeft );
	nTop	= ( ( nTop > nMaximumTop ) ? nMaximumTop : nTop );

	// Move window
	bResult = SetWindowPos( hWndSelectFolder, NULL, nLeft, nTop, 0, 0, ( SWP_NOSIZE | SWP_NOOWNERZORDER ) );

	// Save window position
	RegistrySetValue( SELECT_FOLDER_REGISTRY_TOP_LEVEL_KEY, SELECT_FOLDER_REGISTRY_SUB_KEY, SELECT_FOLDER_REGISTRY_LEFT_VALUE_NAME, nLeft );
	RegistrySetValue( SELECT_FOLDER_REGISTRY_TOP_LEVEL_KEY, SELECT_FOLDER_REGISTRY_SUB_KEY, SELECT_FOLDER_REGISTRY_TOP_VALUE_NAME, nTop );

	return bResult;

} // End of function SelectFolderWindowMove

// Input box window procedure
LRESULT CALLBACK SelectFolderWndProc( HWND hWndSelectFolder, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr = 0;

	static int s_nClickX;
	static int s_nClickY;

	// Select message
	switch( uMessage )
	{
		case WM_CREATE:
		{
			// A create message
			HINSTANCE hInstance;
			int nTitleWindowWidth;
			int nEditWindowWidth;

			// Get instance
			hInstance = ( ( LPCREATESTRUCT )lParam )->hInstance;

			// Calculate title window width
			nTitleWindowWidth = ( SELECT_FOLDER_WINDOW_WIDTH - ( SELECT_FOLDER_WINDOW_SEPARATOR_SIZE + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE ) );

			// Create title window
			g_hWndTitle = CreateWindowEx( SELECT_FOLDER_TITLE_WINDOW_EXTENDED_STYLE, SELECT_FOLDER_TITLE_WINDOW_CLASS_NAME, g_lpszTitle, SELECT_FOLDER_TITLE_WINDOW_STYLE, SELECT_FOLDER_WINDOW_SEPARATOR_SIZE, SELECT_FOLDER_WINDOW_SEPARATOR_SIZE, nTitleWindowWidth, SELECT_FOLDER_TITLE_WINDOW_HEIGHT, hWndSelectFolder, ( HMENU )NULL, hInstance, NULL );

			// Ensure that title window was created
			if( g_hWndTitle )
			{
				// Successfully created title window
				HFONT hFont;
				int nEditWindowHeight;
				int nEditWindowLeft;
				int nEditWindowTop;

				// Get font
				hFont = ( HFONT )GetStockObject( DEFAULT_GUI_FONT );

				// Set title window font
				SendMessage( g_hWndTitle, WM_SETFONT, ( WPARAM )hFont, ( LPARAM )TRUE );

				// Calculate tree view window size
				nEditWindowWidth	= ( SELECT_FOLDER_WINDOW_WIDTH - ( WINDOW_BORDER_WIDTH + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE + WINDOW_BORDER_WIDTH ) );
				nEditWindowHeight	= SELECT_FOLDER_TREE_VIEW_WINDOW_HEIGHT;

				// Calculate tree view window position
				nEditWindowLeft		= SELECT_FOLDER_WINDOW_SEPARATOR_SIZE;
				nEditWindowTop		= ( SELECT_FOLDER_WINDOW_SEPARATOR_SIZE + SELECT_FOLDER_TITLE_WINDOW_HEIGHT + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE );

				// Create tree view window
				g_hWndTreeView = CreateWindowEx( SELECT_FOLDER_TREE_VIEW_WINDOW_EXTENDED_STYLE, SELECT_FOLDER_TREE_VIEW_WINDOW_CLASS_NAME, g_lpszFolderPath, SELECT_FOLDER_TREE_VIEW_WINDOW_STYLE, nEditWindowLeft, nEditWindowTop, nEditWindowWidth, nEditWindowHeight, hWndSelectFolder, ( HMENU )NULL, hInstance, NULL );

				// Ensure that tree view window was created
				if( g_hWndTreeView )
				{
					// Successfully created tree view window
					int nOkButtonWindowLeft;
					int nButtonWindowTop;

					// Set tree view window font
					SendMessage( g_hWndTreeView, WM_SETFONT, ( WPARAM )hFont, ( LPARAM )TRUE );

					// Calculate ok button window position
					nOkButtonWindowLeft	= ( ( nEditWindowLeft + nEditWindowWidth ) - ( SELECT_FOLDER_BUTTON_WINDOW_WIDTH + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE + SELECT_FOLDER_BUTTON_WINDOW_WIDTH ) );
					nButtonWindowTop	= ( nEditWindowTop + SELECT_FOLDER_TREE_VIEW_WINDOW_HEIGHT + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE );

					// Create ok button window
					g_hWndOkButton = CreateWindowEx( SELECT_FOLDER_BUTTON_WINDOW_EXTENDED_STYLE, SELECT_FOLDER_BUTTON_WINDOW_CLASS_NAME, SELECT_FOLDER_OK_BUTTON_WINDOW_TEXT, SELECT_FOLDER_BUTTON_WINDOW_STYLE, nOkButtonWindowLeft, nButtonWindowTop, SELECT_FOLDER_BUTTON_WINDOW_WIDTH, SELECT_FOLDER_BUTTON_WINDOW_HEIGHT, hWndSelectFolder, ( HMENU )SELECT_FOLDER_OK_BUTTON_WINDOW_ID, hInstance, NULL );

					// Ensure that ok button window was created
					if( g_hWndOkButton )
					{
						// Successfully created ok button window
						int nCancelButtonWindowLeft;

						// Set ok button window font
						SendMessage( g_hWndOkButton, WM_SETFONT, ( WPARAM )hFont, ( LPARAM )TRUE );

						// Calculate cancel button window position
						nCancelButtonWindowLeft	= ( nOkButtonWindowLeft + SELECT_FOLDER_BUTTON_WINDOW_WIDTH + SELECT_FOLDER_WINDOW_SEPARATOR_SIZE );

						// Create cancel button window
						g_hWndCancelButton = CreateWindowEx( SELECT_FOLDER_BUTTON_WINDOW_EXTENDED_STYLE, SELECT_FOLDER_BUTTON_WINDOW_CLASS_NAME, SELECT_FOLDER_CANCEL_BUTTON_WINDOW_TEXT, SELECT_FOLDER_BUTTON_WINDOW_STYLE, nCancelButtonWindowLeft, nButtonWindowTop, SELECT_FOLDER_BUTTON_WINDOW_WIDTH, SELECT_FOLDER_BUTTON_WINDOW_HEIGHT, hWndSelectFolder, ( HMENU )SELECT_FOLDER_CANCEL_BUTTON_WINDOW_ID, hInstance, NULL );

						// Ensure that cancel button window was created
						if( g_hWndCancelButton )
						{
							// Successfully created cancel button window

							// Set cancel button window font
							SendMessage( g_hWndCancelButton, WM_SETFONT, ( WPARAM )hFont, ( LPARAM )TRUE );

							// Select tree view text
							SendMessage( g_hWndTreeView, EM_SETSEL, ( WPARAM )0, ( LPARAM )-1 );

							// Focus on tree view window
							SetFocus( g_hWndTreeView );

						} // End of successfully created cancel button window

					} // End of successfully created ok button window

				} // End of successfully created tree view window

			} // End of successfully created title window

			// Break out of switch
			break;

		} // End of a create message
		case WM_ACTIVATE:
		{
			// An activate message

			// Focus on tree view window
			SetFocus( g_hWndTreeView );

			// Break out of switch
			break;

		} // End of an activate message
		case WM_LBUTTONDOWN:
		{
			// A left mouse button down message

			// Set mouse capture to select folder window
			SetCapture( hWndSelectFolder );

			// Store static variables
			s_nClickX = GET_X_LPARAM( lParam );
			s_nClickY = GET_Y_LPARAM( lParam );

			// Break out of switch
			break;

		} // End of a left mouse button down message
		case WM_LBUTTONUP:
		{
			// A left mouse button up message

			// Release mouse capture
			ReleaseCapture();

			// Break out of switch
			break;

		} // End of a left mouse button up message
		case WM_MOUSEMOVE:
		{
			// A mouse move message

			// See if select folder window has mouse capture
			if( GetCapture() == hWndSelectFolder )
			{
				// Input box window has mouse capture
				RECT rc;
				int nMouseX;
				int nMouseY;
				int nWindowX;
				int nWindowY;

				// Get window position
				GetWindowRect( hWndSelectFolder, &rc );

				// Store mouse position
				nMouseX = GET_X_LPARAM( lParam );
				nMouseY = GET_Y_LPARAM( lParam );

				// Calculate window position
				nWindowX = ( ( rc.left + nMouseX ) - s_nClickX );
				nWindowY = ( ( rc.top + nMouseY ) - s_nClickY );

				// Move select folder window
				SelectFolderWindowMove( hWndSelectFolder, nWindowX, nWindowY );

			} // End of select folder window has mouse capture
			else
			{
				// Input box window does not have mouse capture

				// Call default procedure
				lr = DefWindowProc( hWndSelectFolder, uMessage, wParam, lParam );

			} // End of select folder window does not have mouse capture

			// Break out of switch
			break;

		} // End of a mouse move message
		case WM_COMMAND:
		{
			// A command message

			// Select command
			switch( LOWORD( wParam ) )
			{
				case SELECT_FOLDER_OK_BUTTON_WINDOW_ID:
				{
					// Ok button has been pressed

					// Update global input text
					SendMessage( g_hWndTreeView, WM_GETTEXT, ( WPARAM )STRING_LENGTH, ( LPARAM )g_lpszFolderPath );

					// Destroy select folder window
					DestroyWindow( hWndSelectFolder );

					// Break out of switch
					break;

				} // End of ok button has been pressed
				case SELECT_FOLDER_CANCEL_BUTTON_WINDOW_ID:
				{
					// Cancel button has been pressed

					// Destroy select folder window
					DestroyWindow( hWndSelectFolder );

					// Break out of switch
					break;

				} // End of cancel button has been pressed
				default:
				{
					// Default command

					// Call default procedure
					lr = DefWindowProc( hWndSelectFolder, uMessage, wParam, lParam );

					// Break out of switch
					break;

				} // End of default command

			}; // End of selection for command

			// Break out of switch
			break;

		} // End of a command message
		case WM_CTLCOLORSTATIC:
		{
			// A control color static message

			// See if message is from title window
			if( ( HWND )lParam == g_hWndTitle )
			{
				// Message is from title window
				HDC hdcTitle;

				// Get hdc
				hdcTitle = ( HDC )wParam;

				// Set title to transparent
				SetBkMode( hdcTitle, TRANSPARENT );

				// Update return value
				lr = ( LRESULT )CreateSolidBrush( GetSysColor( SELECT_FOLDER_WINDOW_CLASS_BACKGROUND_COLOR ) );

			} // End of message is from title window
			else
			{
				// Message is not from title window

				// Call default window procedure
				lr = DefWindowProc( hWndSelectFolder, uMessage, wParam, lParam );

			} // End of message is not from title window

			// Break out of switch
			break;

		} // End of a control color static message
		case WM_CLOSE:
		{
			// A close message

			// Destroy select folder window
			DestroyWindow( hWndSelectFolder );

			// Break out of switch
			break;

		} // End of a close message
		case WM_DESTROY:
		{
			// A destroy message

			// Terminate thread
			PostQuitMessage( 0 );

			// Break out of switch
			break;

		} // End of a destroy message
		default:
		{
			// Default message

			// Call default window procedure
			lr = DefWindowProc( hWndSelectFolder, uMessage, wParam, lParam );

			// Break out of switch
			break;

		} // End of default message

	}; // End of selection for message

	return lr;

} // End of function SelectFolderWndProc

BOOL TreeViewWindowInit()
{
	BOOL bResult = FALSE;

	// Add drives to tree view window
	if( TreeViewWindowAddDrives() )
	{
		// Successfully added drives to tree view window
	} // End of successfully added drives to tree view window

	return bResult;
	
} // End of function TreeViewWindowInit

BOOL SelectFolder( HWND hWndParent, LPTSTR lpszFolderPath, LPCTSTR lpszTitle )
{
	BOOL bResult = FALSE;

	WNDCLASSEX wcSelectFolder;
	HINSTANCE hInstance;

	// Allocate global memory
	g_lpszFolderPath	= new char[ STRING_LENGTH ];
	g_lpszTitle		= new char[ STRING_LENGTH ];

	// Initialise global strings
	lstrcpy( g_lpszFolderPath,	lpszFolderPath );
	lstrcpy( g_lpszTitle,		lpszTitle );

	// Get instance
	hInstance = GetModuleHandle( NULL );

	// Clear select folder window class structure
	ZeroMemory( &wcSelectFolder, sizeof( wcSelectFolder ) );

	// Initialise select folder window class structure
	wcSelectFolder.cbSize			= sizeof( WNDCLASSEX );
	wcSelectFolder.lpfnWndProc		= SelectFolderWndProc;
	wcSelectFolder.hInstance		= hInstance;
	wcSelectFolder.lpszClassName	= SELECT_FOLDER_WINDOW_CLASS_NAME;
	wcSelectFolder.style			= SELECT_FOLDER_WINDOW_CLASS_STYLE;
	wcSelectFolder.hIcon			= SELECT_FOLDER_WINDOW_CLASS_ICON;
	wcSelectFolder.hCursor			= SELECT_FOLDER_WINDOW_CLASS_CURSOR;
	wcSelectFolder.hbrBackground	= SELECT_FOLDER_WINDOW_CLASS_BACKGROUND;
	wcSelectFolder.lpszMenuName		= SELECT_FOLDER_WINDOW_CLASS_MENU_NAME;
	wcSelectFolder.hIconSm			= SELECT_FOLDER_WINDOW_CLASS_ICON;

	// Register select folder window class
	if( RegisterClassEx( &wcSelectFolder ) )
	{
		// Successfully registered select folder window class
		HWND hWndSelectFolder;

		// Create select folder window
		hWndSelectFolder = CreateWindowEx( SELECT_FOLDER_WINDOW_EXTENDED_STYLE, SELECT_FOLDER_WINDOW_CLASS_NAME, lpszTitle, SELECT_FOLDER_WINDOW_STYLE, SELECT_FOLDER_WINDOW_DEFAULT_LEFT, SELECT_FOLDER_WINDOW_DEFAULT_TOP, SELECT_FOLDER_WINDOW_WIDTH, SELECT_FOLDER_WINDOW_HEIGHT, hWndParent, NULL, hInstance, NULL );

		// Ensure that select folder window was created
		if( hWndSelectFolder )
		{
			// Successfully created select folder window
			MSG msg;
			int nLeft;
			int nTop;

			// Get initial window position
			nLeft	= RegistryGetValue( SELECT_FOLDER_REGISTRY_TOP_LEVEL_KEY, SELECT_FOLDER_REGISTRY_SUB_KEY, SELECT_FOLDER_REGISTRY_LEFT_VALUE_NAME, SELECT_FOLDER_WINDOW_DEFAULT_LEFT );
			nTop	= RegistryGetValue( SELECT_FOLDER_REGISTRY_TOP_LEVEL_KEY, SELECT_FOLDER_REGISTRY_SUB_KEY, SELECT_FOLDER_REGISTRY_TOP_VALUE_NAME, SELECT_FOLDER_WINDOW_DEFAULT_TOP );

			// Move window
			SelectFolderWindowMove( hWndSelectFolder, nLeft, nTop );

			// Clear message structure
			ZeroMemory( &msg, sizeof( msg ) );

			// Show select folder window
			ShowWindow( hWndSelectFolder, SW_SHOW );

			// Update select folder window
			UpdateWindow( hWndSelectFolder );

			// Initialise tree view window
			TreeViewWindowInit();

			// Message loop
			while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
			{
				// Translate message
				TranslateMessage( &msg );

				// Dispatch message
				DispatchMessage( &msg );

			}; // End of message loop

			// See if input text has changed
			if( lstrcmpi( lpszFolderPath, g_lpszFolderPath ) != 0 )
			{
				// Input text has changed

				// Update supplied input text variable
				lstrcpy( lpszFolderPath, g_lpszFolderPath );

				// Update return value
				bResult = TRUE;

			} // End of input text has changed

		} // End of successfully created select folder window
		else
		{
			// Unable to create select folder window

			// Display error message
			MessageBox( NULL, UNABLE_TO_CREATE_SELECT_FOLDER_WINDOW_ERROR_MESSAGE, ERROR_MESSAGE_CAPTION, ( MB_OK | MB_ICONERROR ) );

		} // End of unable to create select folder window

	} // End of successfully registered select folder window class
	else
	{
		// Unable to register select folder window class

		// Display error message
		MessageBox( NULL, UNABLE_TO_REGISTER_SELECT_FOLDER_WINDOW_CLASS_ERROR_MESSAGE, ERROR_MESSAGE_CAPTION, ( MB_OK | MB_ICONERROR ) );

	} // End of unable to register select folder window class

	// Free global memory
	delete [] g_lpszFolderPath;
	delete [] g_lpszTitle;

	return bResult;

} // End of function SelectFolder
