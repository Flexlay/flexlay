/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

#include "precomp.h"
#include <string>
#include "API/GUI/messagebox.h"
#include "API/GUI/stylemanager.h"
#include "API/GUI/gui_manager.h"
#include "API/Core/XML/dom_element.h"
#include "messagebox_generic.h"

/////////////////////////////////////////////////////////////////////////////
// Construction:

CL_MessageBox::CL_MessageBox(
	const std::string &title,
	const std::string &text,
	const std::string &button1,
	const std::string &button2,
	const std::string &button3,
	CL_Component *parent,
	CL_StyleManager *style)
: CL_Window(parent, style), impl(0)
{
	impl = new CL_MessageBox_Generic(this, title, text, button1, button2, button3);
}

CL_MessageBox::~CL_MessageBox()
{
	delete impl;
}

int CL_MessageBox::info(
	const std::string &text,
	CL_Component *parent,
	CL_StyleManager *style)
{
	CL_MessageBox message("Information", text, "Ok", "", "", parent, style);
	message.run();
	return message.get_result_button();
}

int CL_MessageBox::info(
	const std::string &title,
	const std::string &text,
	CL_Component *parent,
	CL_StyleManager *style)
{
	CL_MessageBox message(title, text, "Ok", "", "", parent, style);
	message.run();
	return message.get_result_button();
}

int CL_MessageBox::info(
	const std::string &title,
	const std::string &text,
	const std::string &button1,
	const std::string &button2,
	const std::string &button3,
	CL_Component *parent,
	CL_StyleManager *style)
{
	CL_MessageBox message(title, text, button1, button2, button3, parent, style);
	message.run();
	return message.get_result_button();
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

int CL_MessageBox::get_result_button() const
{
	return impl->result_button;
}
