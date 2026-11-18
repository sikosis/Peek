#include "AboutWindow.h"

#include <AppFileInfo.h>
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <File.h>
#include <Font.h>
#include <IconUtils.h>
#include <InterfaceDefs.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <Resources.h>
#include <Roster.h>
#include <SeparatorView.h>
#include <Size.h>
#include <String.h>
#include <StringView.h>
#include <TabView.h>
#include <View.h>


class IconView : public BView {
public:
	IconView(BBitmap* icon);
	virtual ~IconView();

	virtual void Draw(BRect updateRect);

private:
	BBitmap* fIcon;
};


class LinkView : public BStringView {
public:
	LinkView(const char* name, const char* label, const char* url);

	virtual void AttachedToWindow();
	virtual void MouseDown(BPoint point);

private:
	BString fUrl;
};


static BBitmap* LoadAppIcon() {
	const int32 iconSize = 64;
	BBitmap* icon = new BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), B_RGBA32);
	if (icon->InitCheck() == B_OK) {
		BResources* resources = BApplication::AppResources();
		if (resources != NULL) {
			size_t size = 0;
			const uint8* data = static_cast<const uint8*>(
				resources->LoadResource(B_VECTOR_ICON_TYPE, "BEOS:ICON", &size));
			if (data != NULL && size > 0
				&& BIconUtils::GetVectorIcon(data, size, icon) == B_OK) {
				return icon;
			}
		}
	}
	delete icon;

	app_info appInfo;
	if (be_app->GetAppInfo(&appInfo) != B_OK)
		return NULL;

	BFile appFile(&appInfo.ref, B_READ_ONLY);
	if (appFile.InitCheck() != B_OK)
		return NULL;

	icon = new BBitmap(BRect(0, 0, 31, 31), B_RGBA32);
	if (icon->InitCheck() != B_OK) {
		delete icon;
		return NULL;
	}

	BAppFileInfo appFileInfo(&appFile);
	if (appFileInfo.GetIcon(icon, B_LARGE_ICON) != B_OK) {
		delete icon;
		return NULL;
	}

	return icon;
}
//---------------------------------------------------------------------------------------------------------------------------------//


static BString AppVersion() {
	BString version("Version 0.72");
	app_info appInfo;
	if (be_app->GetAppInfo(&appInfo) != B_OK)
		return version;

	BFile appFile(&appInfo.ref, B_READ_ONLY);
	BAppFileInfo appFileInfo(&appFile);
	version_info versionInfo;
	if (appFileInfo.GetVersionInfo(&versionInfo, B_APP_VERSION_KIND) == B_OK) {
		version.SetToFormat("Version %lu.%lu%lu", (unsigned long)versionInfo.major,
			(unsigned long)versionInfo.middle, (unsigned long)versionInfo.minor);
	}

	return version;
}
//---------------------------------------------------------------------------------------------------------------------------------//


static BStringView* Heading(const char* name, const char* text) {
	BStringView* heading = new BStringView(name, text);
	heading->SetFont(be_bold_font);
	return heading;
}
//---------------------------------------------------------------------------------------------------------------------------------//


static BView* CreateCreditsView() {
	BView* view = new BView("Credits", B_WILL_DRAW);
	view->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	BLayoutBuilder::Group<>(view, B_VERTICAL, 4.0f)
		.SetInsets(10.0f)
		.Add(Heading("author heading", "Original author"))
		.Add(new BStringView("author", "Durand John Miller"))
		.Add(new BStringView("copyright", "Copyright 2001 Durand John Miller"))
		.AddStrut(6.0f)
		.Add(Heading("contributors heading", "Contributors"))
		.Add(new BStringView("contributors", "The Peek and HaikuArchives contributors"))
		.AddStrut(6.0f)
		.Add(new LinkView("source link", "github.com/HaikuArchives/Peek",
			"https://github.com/HaikuArchives/Peek"))
		.AddGlue();

	return view;
}
//---------------------------------------------------------------------------------------------------------------------------------//


static BView* CreateLicenceView() {
	BView* view = new BView("Licence", B_WILL_DRAW);
	view->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	BLayoutBuilder::Group<>(view, B_VERTICAL, 4.0f)
		.SetInsets(10.0f)
		.Add(Heading("licence heading", "Modified BSD licence"))
		.Add(new BStringView("licence summary",
			"Redistribution in source and binary forms is permitted."))
		.Add(new BStringView("licence location",
			"See the bundled license file for the complete terms."))
		.AddGlue();

	return view;
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::IconView(BBitmap* icon)
	: BView("application icon", B_WILL_DRAW), fIcon(icon) {
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetExplicitMinSize(BSize(64, 64));
	SetExplicitMaxSize(BSize(64, 64));
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::~IconView() {
	delete fIcon;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void IconView::Draw(BRect updateRect) {
	(void)updateRect;
	if (fIcon == NULL)
		return;

	SetDrawingMode(B_OP_ALPHA);
	SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	DrawBitmap(fIcon, fIcon->Bounds(), Bounds(), B_FILTER_BITMAP_BILINEAR);
}
//---------------------------------------------------------------------------------------------------------------------------------//


LinkView::LinkView(const char* name, const char* label, const char* url)
	: BStringView(name, label), fUrl(url) {
	SetToolTip(url);
	BFont font;
	GetFont(&font);
	font.SetFace(font.Face() | B_UNDERSCORE_FACE);
	SetFont(&font);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void LinkView::AttachedToWindow() {
	BStringView::AttachedToWindow();
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
	int32 luminance
		= (299 * background.red + 587 * background.green + 114 * background.blue) / 1000;
	if (luminance < 128)
		SetHighColor(105, 190, 255);
	else
		SetHighColor(0, 76, 168);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void LinkView::MouseDown(BPoint point) {
	(void)point;
	const char* arguments[] = { fUrl.String() };
	be_roster->Launch("text/html", 1, arguments);
}
//---------------------------------------------------------------------------------------------------------------------------------//


AboutWindow::AboutWindow()
	: BWindow(BRect(0, 0, 480, 400), "About Peek", B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_CLOSE_ON_ESCAPE) {
	BStringView* title = new BStringView("title", "Peek");
	BFont titleFont(*be_bold_font);
	titleFont.SetSize(be_plain_font->Size() * 1.7f);
	title->SetFont(&titleFont);

	BString versionText = AppVersion();
	BStringView* version = new BStringView("version", versionText.String());
	BStringView* description = new BStringView("description",
		"A small, fast image browser and viewer for Haiku.");

	BTabView* tabs = new BTabView("about tabs", B_WIDTH_FROM_LABEL);
	tabs->AddTab(CreateCreditsView());
	tabs->AddTab(CreateLicenceView());

	BButton* closeButton
		= new BButton("close", "Close", new BMessage(B_QUIT_REQUESTED));
	closeButton->MakeDefault(true);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 8.0f)
		.SetInsets(14.0f)
		.AddGroup(B_HORIZONTAL, 12.0f)
			.Add(new IconView(LoadAppIcon()))
			.AddGroup(B_VERTICAL, 3.0f)
				.Add(title)
				.Add(version)
				.AddStrut(3.0f)
				.Add(description)
				.AddGlue()
			.End()
		.End()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.Add(tabs, 1.0f)
		.AddGroup(B_HORIZONTAL, 0.0f)
			.AddGlue()
			.Add(closeButton)
		.End();

	CenterOnScreen();
}
//---------------------------------------------------------------------------------------------------------------------------------//
