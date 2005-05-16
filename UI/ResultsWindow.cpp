///////////////////////////////////////////////////////////////////////////////
// Application : MoleSVN
// Package     : UI
// File        : ResultsWindow.cpp
// Description : implements window which will display svn command results
// Author      : cedric.bresson@artcoder.com
///////////////////////////////////////////////////////////////////////////////

#include "ResultsWindow.h"
#include "../MoleSvnAddon.h"
#include "MessageCommands.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// -- ResultsWindow
///////////////////////////////////////////////////////////////////////////////
ResultsWindow::ResultsWindow(const string& strTitle)
:BWindow(BRect(0,0,100,100), strTitle.c_str(), B_DOCUMENT_WINDOW, 0), m_pView(NULL)
{
	TRACE_METHOD ((CC_APPLICATION, REPORT_METHOD));

	// In the future, we must retrieve the old position and size from preferences
	// Compute window position and size	
	BScreen screen;
	BRect screenFrame = screen.Frame();
	const float fWindowWidth = 400.0f;
	const float fWindowHeight = 300.0f;
	MoveTo((screenFrame.Width() - fWindowWidth) / 2.0f, (screenFrame.Height() - fWindowHeight) / 2.0f);
	ResizeTo(fWindowWidth, fWindowHeight);

	m_pView = new ResultsView(BRect(0,0,Frame().Width(), Frame().Height()));
	m_pView->SetViewColor(216, 216, 216);
	
	AddChild(m_pView);
	Show();
	MoleSvnAddon::GetInstance()->AddThread(Thread());
}
	
ResultsWindow::~ResultsWindow()
{
	TRACE_METHOD ((CC_APPLICATION, REPORT_METHOD));

	MoleSvnAddon::GetInstance()->RemoveThread(Thread());
}

void ResultsWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case MSG_SVN_STDOUT:
		{
			const char* buf;
			message->FindString("text", 0, &buf);
			m_pView->AddItem(string(buf));	
			break;
		}
		case MSG_CMD_FINISHED:
		{
			m_pView->GetOkButton()->SetEnabled(true);
			break;
		}
		case MSG_OK:
		{
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

bool ResultsWindow::QuitRequested()
{
	TRACE_METHOD ((CC_APPLICATION, REPORT_METHOD));

	// stop svn execution
	
	return m_pView->GetOkButton()->IsEnabled();
}


///////////////////////////////////////////////////////////////////////////////
// -- ResultsView
///////////////////////////////////////////////////////////////////////////////
ResultsView::ResultsView(BRect frame)
:BView(BRect(frame), 
	   "ResultsWindowView",
	   B_FOLLOW_ALL_SIDES,
	   B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE)
{
	TRACE_METHOD ((CC_APPLICATION, REPORT_METHOD));
	
	// ListView
	BRect ListViewRect(g_fSpaceToWindowBorder,
	                   g_fSpaceToWindowBorder,
	                   frame.Width() - (g_fSpaceToWindowBorder + B_V_SCROLL_BAR_WIDTH),
	                   frame.Height() - (g_fSpaceToWindowBorder + g_fButtonHeight + g_fSpaceToWindowBorder + B_H_SCROLL_BAR_HEIGHT) );
	m_pListView = new BListView(ListViewRect, 
	                            "ResultWindow_ListView",
	                            B_SINGLE_SELECTION_LIST,
	                            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
	                            B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE);

	AddChild(new BScrollView("ResultWindows_ScrollView", 
	                         m_pListView,
	                         B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
	                         B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE,
	                         true,
	                         true));

	// Ok button
	float fButtonX = frame.IntegerWidth() - (g_fButtonWidth + g_fSpaceToWindowBorder);
	float fButtonY = frame.IntegerHeight() - (g_fButtonHeight + g_fSpaceToWindowBorder);
	m_pOk = new BButton(BRect(fButtonX, fButtonY, fButtonX + g_fButtonWidth, fButtonY + g_fButtonHeight),
		    				  "ResultsWindow_Ok", 
	                          "Ok",
	                          new BMessage(MSG_OK),
	                          B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	m_pOk->MakeDefault(true);
	m_pOk->SetEnabled(false);
	AddChild(m_pOk);

	// Cancel button
	fButtonX -= g_fButtonWidth + g_fButtonsSpace;
	m_pCancel = new BButton(BRect(fButtonX, fButtonY, fButtonX + g_fButtonWidth, fButtonY + g_fButtonHeight),
		    				  "ResultsWindow_Cancel", 
	                          "Cancel",
	                          new BMessage(MSG_CANCEL),
	                          B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	m_pCancel->SetEnabled(false);
	AddChild(m_pCancel);
}
	
ResultsView::~ResultsView()
{
	TRACE_METHOD ((CC_APPLICATION, REPORT_METHOD));

}

BButton* ResultsView::GetOkButton()
{
	return m_pOk;
}

void ResultsView::AddItem(const std::string& strText)
{
	m_pListView->AddItem(new BStringItem(strText.c_str()));
	int nItems = m_pListView->CountItems();
	
	TRACE_SIMPLE ((CC_APPLICATION, CR_INFO, "m_pListView->CurrentSelection() : %d", m_pListView->CurrentSelection()));
	TRACE_SIMPLE ((CC_APPLICATION, CR_INFO, "m_pListView->CountItems() : %d", m_pListView->CountItems()));
	
	// We have just on item, so we select it
	if(nItems == 1)
		m_pListView->Select(0);	
	else if(nItems == m_pListView->CurrentSelection()+2)
	{
		m_pListView->Select(nItems-1);
		m_pListView->ScrollToSelection();
	}
}


