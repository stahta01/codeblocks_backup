#ifndef WXSPALETTE_H
#define WXSPALETTE_H

#include <wx/panel.h>
#include <wx/radiobut.h>

class wxSmith;
class wxsWidget;
class wxsResource;

class wxsPalette : public wxPanel
{
	public:
	
        static const int itBefore = 0x01;
        static const int itAfter  = 0x02;
        static const int itInto   = 0x04;
	
        /** Constructor */
		wxsPalette(wxWindow* Parent,wxSmith* Plugin,int PageNum);
		
		/** Destructor */
		virtual ~wxsPalette();
		
		/** Getting singleton object - one palette inside whole plugin*/
		static wxsPalette* Get() { return Singleton; }
		
        /** Getting current insertion type */
        int GetInsertionType();
        
        /** Setting mask for insertion type */
        void SetInsertionTypeMask(int Mask);
        
        /** Selecting given resource */
        void SelectResource(wxsResource* Res);
        
        /** Notifying that given resource was closed */
        void ResourceClosed(wxsResource* Res);
		
	private:
	
        /** Plugin object */
        wxSmith* Plugin;
        
        /** Currently selected resource */
        wxsResource* SelectedRes;
        
        /* Checkboxes */
        wxRadioButton* AddBefore;
        wxRadioButton* AddAfter;
        wxRadioButton* AddInto;
        
        /** Current insertion type */
        int InsType;
        
        /** Current insertion type mask */
        int InsTypeMask;
        
        /** Page number */
        int PageNum;
        
        /** Singleton object */
        static wxsPalette* Singleton;
        
        /** Funcntion creating palette of widgets in given window */
        void CreateWidgetsPalette(wxWindow* Wnd);
        
        /** Function inserting widget with giwen name */
        void InsertRequest(const char* Name);
        
        /** Function adding new widget before given one */
        void InsertBefore(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function adding new widget after given one */
        void InsertAfter(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function adding new widget into given one */
        void InsertInto(wxsWidget* NewWidget,wxsWidget* Reference);
        
        /** Function deleting currently selected widget */
        void DeleteRequest();
        
        /** Function showing preview of currently selected resource */
        void PreviewRequest();
        
        /* Event-processing functions */
        void OnRadio(wxCommandEvent& event);
        void OnButton(wxCommandEvent& event);
        
        
        DECLARE_EVENT_TABLE()
        
};

#endif // WXSPALETTE_H
