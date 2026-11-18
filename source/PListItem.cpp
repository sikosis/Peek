#include "PListItem.h"

#include <InterfaceDefs.h>
#include <string.h>

PListItem::PListItem(BEntry *e, const char *name) : BListItem()
{
 theFile = NULL;
 theFile = new entry_ref;
 e->GetRef(theFile);
 if (name == NULL) e->GetName(theName);
 else strcpy(theName,name);  
 if (e->IsDirectory()) isDirectory = true;
    else isDirectory = false;
}

PListItem::PListItem(entry_ref e, bool isD, const char *name) : BListItem()
{
 theFile = NULL;
 theFile = new entry_ref;
 *theFile = e;
 if (name == NULL) strcpy( theName, e.name);
  else strcpy(theName,name);  
 isDirectory = isD;
}


PListItem::~PListItem() {
  if (theFile != NULL) delete theFile;
}

// GetEntry - requires that e declared before the method is entered!
//            the BEntry is yours, so you delete it and stuff.
int32 PListItem::GetEntry(BEntry *e) {
 if (theFile == NULL) return -1;
 e->SetTo(theFile,true);
 return B_OK;
}

// Ref() -- returns the actual ref
//          Don't delete it, dude.
entry_ref PListItem::Ref() {
  return *theFile;
}



void PListItem::DrawItem(BView *owner, BRect frame, bool complete) {
  owner->SetDrawingMode(B_OP_COPY);
  color_which background = IsSelected()
    ? B_LIST_SELECTED_BACKGROUND_COLOR : B_LIST_BACKGROUND_COLOR;
  owner->SetLowColor(ui_color(background));
  if (complete || IsSelected()) {
    owner->SetHighColor(ui_color(background));
    owner->FillRect(frame);
  }

  color_which textColour;
  if (IsSelected())
    textColour = B_LIST_SELECTED_ITEM_TEXT_COLOR;
  else if (isDirectory)
    textColour = B_LINK_TEXT_COLOR;
  else
    textColour = B_LIST_ITEM_TEXT_COLOR;
  owner->SetHighColor(ui_color(textColour));
  owner->MovePenTo(frame.left + (isDirectory ? 4 : 10), frame.bottom - 2);
  owner->DrawString(theName);
}
//---------------------------------------------------------------------------------------------------------------------------------//
