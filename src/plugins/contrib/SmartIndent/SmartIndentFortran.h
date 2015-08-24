#ifndef SMARTINDENTFORTRAN_H_INCLUDED
#define SMARTINDENTFORTRAN_H_INCLUDED


#include <cbplugin.h> // for "class cbSmartIndentPlugin"

class cbEditor;
class wxScintillaEvent;

class SmartIndentFortran : public cbSmartIndentPlugin
{
    public:
        virtual void OnEditorHook(cbEditor* ed, wxScintillaEvent& event) const;
};

#endif // SMARTINDENTFORTRAN_H_INCLUDED
