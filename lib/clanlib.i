%include "std_string.i"

class CL_Component
{
private:
	~CL_Component();
public:
	CL_Component(CL_Component* parent, CL_StyleManager* style = NULL);
        void show(bool show = true);
       	bool is_visible(bool check_parents = true);
	void set_size(int new_width, int new_height);
	void set_position(int new_x, int new_y);
	const CL_Rect& get_position();
};

class CL_Size
{
public:
	CL_Size(int w, int h);
	int width;
	int height;
};

class CL_Pointf
{
public:
	CL_Pointf(float x, float y)
		: x(x), y(y) { }
	float x;
	float y;
};


class CL_Point
{
public:
	CL_Point(int x, int y)
		: x(x), y(y) { }
	int x;
	int y;
};

class CL_Rect
{
public:
	int left;
	int right;
	int top;
	int bottom;

	CL_Rect(int, int, int, int);
	CL_Rect(const CL_Point&, const CL_Size&);     

	int get_width() const;
	int get_height() const;

	CL_Size get_size();
};

class CL_Color
{
public:
	CL_Color(unsigned int, unsigned int, unsigned int, unsigned int = 255);

	void set_red  (unsigned int);
	void set_blue (unsigned int);
	void set_green(unsigned int);
	void set_alpha(unsigned int);

	unsigned int get_red  ();
	unsigned int get_blue ();
	unsigned int get_green();
	unsigned int get_alpha();
};

class CL_Window : public CL_Component
{
private:
	~CL_Window();
public:
	CL_Window(
		const CL_Rect &pos,
		const std::string &title,
		CL_Component *parent,
		CL_StyleManager *style = NULL);
	CL_Component* get_client_area();
};

class CL_Button : public CL_Component
{
private:
	~CL_Button();
public:	
	CL_Button(
		const CL_Rect &pos,
		const std::string &text,
		CL_Component *parent,
		CL_StyleManager *style = NULL);

	CL_Signal_v0 &sig_clicked();
};

class CL_Menu : public CL_Component
{
private:
	~CL_Menu();
public:
	CL_Menu(
		const CL_Rect &rect,
		CL_Component *parent,
		CL_StyleManager *style = NULL,
		bool vertical=false);

	CL_Menu(
		CL_Component *parent,
		CL_StyleManager *style = NULL,
		bool vertical=false);
	
	CL_MenuNode *create_item( const std::string &path, const std::string &labels=std::string());
};

class CL_MenuNode : public CL_Component
{
private:
	~CL_MenuNode();
//! Construction:
public:
	//: CL_MenuNode Constructor
	CL_MenuNode(
		CL_Menu *parent_menu,
		CL_StyleManager *style = NULL);
	CL_Signal_v0 &sig_clicked();
};

class CL_InputBox : public CL_Component
{
public:
	CL_InputBox(
		const CL_Rect &pos,
		CL_Component *parent,
		CL_StyleManager *style = NULL);

	void set_text(const std::string &text);
	const std::string &get_text() const;        
};

class CL_Label : public CL_Component
{
public:
		//: Label Constructor
	CL_Label(
		const CL_Point &pos,
		const std::string &text,
		CL_Component *parent,
		CL_StyleManager *style = NULL);
};

class CL_ListBox : public CL_Component
{
public:
	CL_ListBox(     
		const CL_Rect& pos, CL_Component* parent, CL_StyleManager* style = NULL);
       	CL_Signal_v1<int>& sig_highlighted();   
	int insert_item(
		const std::string& text, int index = -1);
};

class CL_ResourceManager
{
public:
	CL_ResourceManager(
		const std::string &config_file,
		CL_InputSourceProvider *provider = 0,
		bool delete_inputsource_provider = false);
	~CL_ResourceManager();

	std::list<std::string> get_all_resources(const std::string &section_name);
	std::list<std::string> get_all_sections();

	std::list<std::string> get_resources_of_type(const std::string &type_id);
	std::list<std::string> get_resources_of_type(const std::string &type_id, const std::string &section_name);
	void add_resources(const CL_ResourceManager &additional_resources);
	void remove_resources(const CL_ResourceManager &additional_resources);
};


/* EOF */

