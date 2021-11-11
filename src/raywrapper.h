#pragma once

#include "variable_defines.h"

#include "memory.h"

// #define RL_MALLOC(sz)       GET_DATA(sz)
// #define RL_CALLOC(n,sz)     GET_CLEAR_DATA(n,sz)
// #define RL_REALLOC(ptr,sz)  GROW_DATA(ptr,sz)
// #define RL_FREE(ptr)        FREE_DATA(ptr)

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

enum Editor_Mode_Settings
{
    EDITOR_MODE_GAME = 0,
    EDITOR_MODE_UI,
    EDITOR_MODE_DEBUG,
    EDITOR_MODE_SETTINGS
};

enum Editor_Control_Settings
{
    EDITOR_CONTROL_PLAY = 0,
    EDITOR_CONTROL_EDIT,
    EDITOR_CONTROL_SETTINGS
};

struct Game_State
{
    Editor_Mode_Settings editor_Mode_Settings;
    Editor_Control_Settings editor_Control_Settings;
};

struct System_State
{
    int screen_Width;
    int screen_Height;
    int fps;
    Color clear_Color;
    const char* window_Title;
    Camera2D camera;
};

FUNC_SIG void SystemStateSetup(System_State *system_State)
{
    system_State->window_Title = "Game";
    system_State->screen_Width = 1920;
    system_State->screen_Height = 1080;
    system_State->clear_Color = {255, 0, 255, 255}; // hot pink
    system_State->fps = 120;
    system_State->camera = { {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f };
}

FUNC_SIG void SetupRaylib(System_State *system_State)
{
    InitWindow(system_State->screen_Width, system_State->screen_Height, system_State->window_Title);
    SetTargetFPS(system_State->fps);

    GuiLoadStyle("assets/styles/cyber/cyber.rgs");

    SetExitKey(0);
}

FUNC_SIG void CleanupRaylib()
{
    CloseWindow();
}

enum Debug_Types
{
    CARD_TYPE,
    WINDOW_TYPE,

    VEC2_TYPE,
    VEC3_TYPE,
    RECT_TYPE,
    COLOR_TYPE,

    CHAR_TYPE,
    INT32_TYPE,
    UINT32_TYPE,
    FLOAT32_TYPE,
    BOOL_TYPE,
    STR_TYPE,
};

struct Debug_Item
{
    const char *name;
    Rectangle *rect;
    Debug_Types type;
    char *entity;

    int gui_Entity_Count;
    char** gui_Entities;
};

struct Debug_Items
{
    int item_Count;
    Debug_Item items[256];
    int item_Hover;
    int item_Selected;
};

struct Gui_Vector2
{
    Gui_Control group;
    Gui_Control labels[3];
};

struct Gui_Window
{
    char* title;
    Vector2 position;
    Vector2 open_Size;
    Rectangle rectangle;
    Vector2 viewScroll;

    bool hover;
    bool clicked;

    bool open;
    bool alive;
    bool drag;
};

FUNC_SIG void InitGuiWIndow(Gui_Window gui_Window)
{
    gui_Window = {0};

    gui_Window.alive = true;
    gui_Window.open = true;
    gui_Window.drag = false;

    gui_Window.hover = false;
    gui_Window.clicked = false;

    gui_Window.title = (char*)"#198# New Window";
    gui_Window.position = { 0.0f, 0.0f };
    gui_Window.open_Size = { 100.0f, 100.0f};
    gui_Window.rectangle = { 0.0f, 0.0f, 100.0f, 100.0f};

    gui_Window.viewScroll = {100.0f, 100.0f};
}

struct Debug_Window
{
    bool alive = false;

    Rectangle window;

    char allTextBox[12][64];
    char *previousTextBox;
    char *activeTextBox;
    int command_Count;

    Rectangle text_Box;
};

#define MAX_DEBUG_LINES 256
struct Debug_Lines
{
    int line_Count;
    Vector2 start_Lines[MAX_DEBUG_LINES];
    Vector2 end_Lines[MAX_DEBUG_LINES];
    Color line_Colors[MAX_DEBUG_LINES];
};
Debug_Lines debug_Lines = {0};

FUNC_SIG int DEBUG_GetLine()
{
    if(debug_Lines.line_Count >= MAX_DEBUG_LINES)
        return -1;

    ++debug_Lines.line_Count;
    return debug_Lines.line_Count - 1;
}

FUNC_SIG void DEBUG_AddLine(int index, Vector2 * start, Vector2 * end, Color color)
{
    if(index > MAX_DEBUG_LINES)
        return;

    debug_Lines.start_Lines[index].x = start->x;
    debug_Lines.start_Lines[index].y = start->y;
    debug_Lines.end_Lines[index].x = end->x;
    debug_Lines.end_Lines[index].y = end->y;
    debug_Lines.line_Colors[index] = color;
}

FUNC_SIG void DEBUG_DrawLines()
{
    if(debug_Lines.line_Count > MAX_DEBUG_LINES)
    {
        debug_Lines.line_Count = MAX_DEBUG_LINES;
    }

    for(int i = 0; i < debug_Lines.line_Count; ++i)
    {
        DrawLineEx(debug_Lines.start_Lines[i], debug_Lines.end_Lines[i], 5, debug_Lines.line_Colors[i]);
    }
}

FUNC_SIG void MoveEntityData(Debug_Item *debug_Item, Vector2 position)
{
    switch(debug_Item->type)
    {
        case CARD_TYPE:
        {
            Visual_Card *temp_Card = (Visual_Card*)debug_Item->entity;
            temp_Card->rectangle.x = position.x;
            temp_Card->rectangle.y = position.y;
        }break;
    }
}

FUNC_SIG void SetupDebugGuiData(char *debug_Item, Debug_Types type, const char *name, char** gui_Entities)
{
    switch(type)
    {
        case CARD_TYPE:
        {
            Visual_Card *temp_Card = (Visual_Card*)debug_Item;
            SetupDebugGuiData((char*)(&temp_Card->rectangle), RECT_TYPE, "Card Rect", &gui_Entities[0]);
            SetupDebugGuiData((char*)(&temp_Card->center), VEC2_TYPE, "Card Center Point", &gui_Entities[4]);
            SetupDebugGuiData((char*)(&temp_Card->collidable), BOOL_TYPE, "Collidable", &gui_Entities[6]);
            SetupDebugGuiData((char*)(&temp_Card->color), COLOR_TYPE, "Card Color", &gui_Entities[7]);
            SetupDebugGuiData((char*)(&temp_Card->font_Height), FLOAT32_TYPE, "Font Height", &gui_Entities[8]);
            SetupDebugGuiData((char*)(&temp_Card->stack_Value), INT32_TYPE, "Stack Index", &gui_Entities[9]);
            SetupDebugGuiData((char*)(&temp_Card->card_Index), INT32_TYPE, "Card Index", &gui_Entities[10]);
        }break;
        case VEC2_TYPE:
        {
            Vector2 *temp_Rect = (Vector2 *)debug_Item;
            SetupDebugGuiData((char*)(&temp_Rect->x), FLOAT32_TYPE, "X", &gui_Entities[0]);
            SetupDebugGuiData((char*)(&temp_Rect->y), FLOAT32_TYPE, "Y", &gui_Entities[1]);
        }break;
        case VEC3_TYPE:
        {
            Vector3 *temp_Rect = (Vector3 *)debug_Item;
            SetupDebugGuiData((char*)(&temp_Rect->x), FLOAT32_TYPE, "X", &gui_Entities[0]);
            SetupDebugGuiData((char*)(&temp_Rect->y), FLOAT32_TYPE, "Y", &gui_Entities[1]);
            SetupDebugGuiData((char*)(&temp_Rect->z), FLOAT32_TYPE, "Z", &gui_Entities[2]);
        }break;
        case RECT_TYPE:
        {
            Rectangle *temp_Rect = (Rectangle *)debug_Item;
            SetupDebugGuiData((char*)(&temp_Rect->x), FLOAT32_TYPE, "X", &gui_Entities[0]);
            SetupDebugGuiData((char*)(&temp_Rect->y), FLOAT32_TYPE, "Y", &gui_Entities[1]);
            SetupDebugGuiData((char*)(&temp_Rect->width), FLOAT32_TYPE, "W", &gui_Entities[2]);
            SetupDebugGuiData((char*)(&temp_Rect->height), FLOAT32_TYPE, "H", &gui_Entities[3]);
        }break;
        case COLOR_TYPE:
        {
            Color *color = 0;
            STORE_PERM_DATA(color, sizeof(Color));
            gui_Entities[0] = (char*)color;
        }break;
        case FLOAT32_TYPE:
        {
            Gui_EditValueFloatControl *value_Float = 0;
            STORE_PERM_DATA(value_Float, sizeof(Gui_EditValueFloatControl));
            SetupGuiValueBox(value_Float);
            value_Float->value = (float*)debug_Item;
            value_Float->text = name;
            gui_Entities[0] = (char*)value_Float;
        }break;
        case INT32_TYPE:
        {
            Gui_EditValueControl *value_Int = 0;
            STORE_PERM_DATA(value_Int, sizeof(Gui_EditValueControl));
            SetupGuiValueBox(value_Int);
            value_Int->value = *((int*)debug_Item);
            value_Int->text = name;
            gui_Entities[0] = (char*)value_Int;
        }break;
        case BOOL_TYPE:
        {
            Gui_EditActiveControl *value_Toggle = 0;
            STORE_PERM_DATA(value_Toggle, sizeof(Gui_EditActiveControl));
            value_Toggle->bounds = {0};
            value_Toggle->text = name;
            value_Toggle->active = *((bool*)debug_Item);
            value_Toggle->edit_Mode = false;
            gui_Entities[0] = (char*)value_Toggle;
        }break;
    }
}

FUNC_SIG void SetupDebugGuiData(Debug_Item *debug_Item)
{
    switch(debug_Item->type)
    {
        case CARD_TYPE:
        {
            Visual_Card *temp_Card = (Visual_Card*)debug_Item->entity;
            debug_Item->gui_Entities = 0;
            debug_Item->gui_Entity_Count = 11;

            STORE_PERM_DATA(debug_Item->gui_Entities, sizeof(Gui_EditValueFloatControl*) *debug_Item->gui_Entity_Count);
 
            SetupDebugGuiData((char*)(&temp_Card->rectangle), RECT_TYPE, "Card Rect", &(debug_Item->gui_Entities[0]));
            SetupDebugGuiData((char*)(&temp_Card->center), VEC2_TYPE, "Card Center Point", &(debug_Item->gui_Entities[4]));
            SetupDebugGuiData((char*)(&temp_Card->collidable), BOOL_TYPE, "Collidable", &(debug_Item->gui_Entities[6]));
            SetupDebugGuiData((char*)(&temp_Card->color), COLOR_TYPE, "Card Color", &(debug_Item->gui_Entities[7]));
            SetupDebugGuiData((char*)(&temp_Card->font_Height), FLOAT32_TYPE, "Font Height", &(debug_Item->gui_Entities[8]));
            SetupDebugGuiData((char*)(&temp_Card->stack_Value), INT32_TYPE, "Stack Value", &(debug_Item->gui_Entities[9]));
            SetupDebugGuiData((char*)(&temp_Card->card_Index), INT32_TYPE, "Card Index", &(debug_Item->gui_Entities[10]));
        }break;
        case WINDOW_TYPE:
        {
            Gui_Window *temp_Window = (Gui_Window*)debug_Item->entity;
            debug_Item->gui_Entities = 0;
            debug_Item->gui_Entity_Count = 6;
            STORE_PERM_DATA(debug_Item->gui_Entities, sizeof(Gui_EditValueFloatControl*) *debug_Item->gui_Entity_Count);
            
            SetupDebugGuiData((char*)(&temp_Window->position), VEC2_TYPE, "Card Center Point", &(debug_Item->gui_Entities[0]));
            SetupDebugGuiData((char*)(&temp_Window->rectangle), RECT_TYPE, "Card Rect", &(debug_Item->gui_Entities[2]));
        }break;
    }
}

FUNC_SIG void DrawType(Rectangle window_Slice, char *entity, Debug_Types type, char** gui_Entities)
{
    float x_Edge = 5.0f;
    float y_Edge = 5.0f;

    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textBox_Padding = GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)  + GuiGetStyle(TEXTBOX, TEXT_LINES_PADDING);
    float total = text_Height + textBox_Padding + 10.0f;

    float vec_Width = (window_Slice.width - x_Edge * 3.0f - textBox_Padding * 2.0f) / 2.0f;
    float vec_Height = (window_Slice.height - y_Edge * 3.0f) / 2.0f;
    
    int item_Height = 1;
    float local_Height = 0.0f;

    switch(type)
    {
        case CARD_TYPE:
        {
            Rectangle rect_Rect = { window_Slice.x + x_Edge, window_Slice.y + local_Height + y_Edge * (float)(item_Height),
                                   window_Slice.width - 2.0f * x_Edge, total * 2.0f };
            local_Height += rect_Rect.height;
            ++item_Height;
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, entity, RECT_TYPE, gui_Entities);

            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.height = total;
            local_Height += rect_Rect.height;
            ++item_Height;
            GuiGroupBox(rect_Rect, "Center");
            DrawType(rect_Rect, entity, VEC2_TYPE, &gui_Entities[4]);

            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.height = total;
            local_Height += rect_Rect.height;
            ++item_Height;
            DrawType(rect_Rect, entity, BOOL_TYPE, &gui_Entities[6]);
            ((Visual_Card *)entity)->collidable = ((Gui_EditActiveControl*)gui_Entities[6])->active;

            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.height = 202.0f;
            local_Height += rect_Rect.height;
            ++item_Height;
            *((Color*)gui_Entities[7]) = ((Visual_Card *)entity)->color;
            DrawType(rect_Rect, entity, COLOR_TYPE, &gui_Entities[7]);
            ((Visual_Card *)entity)->color = *((Color*)gui_Entities[7]);
            
            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.width = window_Slice.width - 2.0f * x_Edge;
            rect_Rect.height = total;
            local_Height += rect_Rect.height;
            ++item_Height;
            DrawType(rect_Rect, entity, FLOAT32_TYPE, &gui_Entities[8]);
            
            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.width = window_Slice.width - 2.0f * x_Edge;
            rect_Rect.height = total;
            local_Height += rect_Rect.height;
            ++item_Height;
            DrawType(rect_Rect, entity, INT32_TYPE, &gui_Entities[9]);
            
            rect_Rect.y = window_Slice.y + local_Height + y_Edge * (float)(item_Height);
            rect_Rect.width = window_Slice.width - 2.0f * x_Edge;
            rect_Rect.height = total;
            local_Height += rect_Rect.height;
            ++item_Height;
            DrawType(rect_Rect, entity, INT32_TYPE, &gui_Entities[10]);
        }break;
        case WINDOW_TYPE:
        {
            Rectangle rect_Rect = { window_Slice.x + x_Edge, window_Slice.y + y_Edge,  window_Slice.width - 2.0f * x_Edge, total };
            GuiGroupBox(rect_Rect, "Position");
            DrawType(rect_Rect, entity, VEC2_TYPE, gui_Entities);

            rect_Rect.y = window_Slice.y + rect_Rect.height + y_Edge * 2.0f;
            rect_Rect.width = window_Slice.width - 2.0f * x_Edge;
            rect_Rect.height = total * 2.0f;
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, entity, RECT_TYPE, &gui_Entities[2]);
        }break;
        case COLOR_TYPE:
        {
            Color *temp_Color = (Color*)(gui_Entities[0]);
            *temp_Color = GuiColorPicker({ window_Slice.x + text_Height * 5.0f + textBox_Padding + 320.0f, window_Slice.y, 196.0f, 192.0f }, *temp_Color);
        }break;
        case BOOL_TYPE:
        {
            Gui_EditActiveControl *temp_boolToggleControl = (Gui_EditActiveControl*)(gui_Entities[0]);

            temp_boolToggleControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding, 
                window_Slice.y, 
                15.0f , 
                window_Slice.height 
            };

            temp_boolToggleControl->active = GuiCheckBox(temp_boolToggleControl);
        }break;
        case FLOAT32_TYPE:
        {
            Gui_EditValueFloatControl *temp_FloatControl = (Gui_EditValueFloatControl*)(gui_Entities[0]);

            temp_FloatControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding,
                window_Slice.y,
                window_Slice.width - textBox_Padding - text_Height * 5.0f ,
                window_Slice.height
            };

            if(GuiValueBox(temp_FloatControl))
            {
                temp_FloatControl->edit_Mode = !temp_FloatControl->edit_Mode;
            }
        }break;
        case INT32_TYPE:
        {
            Gui_EditValueControl *temp_IntControl = (Gui_EditValueControl*)(gui_Entities[0]);

            temp_IntControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding,
                window_Slice.y,
                window_Slice.width - textBox_Padding - text_Height * 5.0f ,
                window_Slice.height
            };

            if(GuiValueBox(temp_IntControl))
            {
                temp_IntControl->edit_Mode = !temp_IntControl->edit_Mode;
            }
        }break;
        case RECT_TYPE:
        {
            Gui_EditValueFloatControl *temp_RectX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_RectY = (Gui_EditValueFloatControl*)(gui_Entities[1]);
            Gui_EditValueFloatControl *temp_RectW = (Gui_EditValueFloatControl*)(gui_Entities[2]);
            Gui_EditValueFloatControl *temp_RectH = (Gui_EditValueFloatControl*)(gui_Entities[3]);

            temp_RectX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                                  window_Slice.y + y_Edge, 
                                  vec_Width, 
                                  vec_Height};
            if(GuiValueBox(temp_RectX))
            {
                temp_RectX->edit_Mode = !temp_RectX->edit_Mode;
            }

            temp_RectY->bounds = temp_RectX->bounds;
            temp_RectY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_RectY))
            {
                temp_RectY->edit_Mode = !temp_RectY->edit_Mode;
            }

            temp_RectW->bounds = temp_RectX->bounds;
            temp_RectW->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectW))
            {
                temp_RectW->edit_Mode = !temp_RectW->edit_Mode;
            }

            temp_RectH->bounds = temp_RectX->bounds;
            temp_RectH->bounds.x += x_Edge + textBox_Padding + vec_Width;
            temp_RectH->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectH))
            {
                temp_RectH->edit_Mode = !temp_RectH->edit_Mode;
            }
        }break;
        case VEC2_TYPE:
        {
            Gui_EditValueFloatControl *temp_VecX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_VecY = (Gui_EditValueFloatControl*)(gui_Entities[1]);

            temp_VecX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                               window_Slice.y + y_Edge, 
                               vec_Width, 
                               window_Slice.height - y_Edge * 2.0f};
            if(GuiValueBox(temp_VecX))
            {
                temp_VecX->edit_Mode = !temp_VecX->edit_Mode;
            }

            temp_VecY->bounds = temp_VecX->bounds;
            temp_VecY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_VecY))
            {
                temp_VecY->edit_Mode = !temp_VecY->edit_Mode;
            }
        }break;
    }
}

FUNC_SIG void DebugEditor(Gui_Window *editor_Window, Debug_Item * debug_Item)
{
    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE) / 2.0f;
    int statusBarHeight = WINDOW_STATUSBAR_HEIGHT + 2;
    int border_Width = 2 * GuiGetStyle(STATUSBAR, BORDER_WIDTH);
    int slider_Size = GuiGetStyle(SLIDER, SLIDER_WIDTH);

    float x_Edge = 5.0f;
    float y_Edge = 15.0f;

    Rectangle view = GuiScrollPanel( 
        { editor_Window->rectangle.x + x_Edge, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        editor_Window->rectangle.width - 10.0f, 
        editor_Window->rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT}, 
        
        { editor_Window->rectangle.x + 5.0f, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        (float)GetScreenWidth(), 
        (float)GetScreenHeight() }, &editor_Window->viewScroll);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    Rectangle window_Slice = 
    {
        editor_Window->rectangle.x + x_Edge,
        editor_Window->rectangle.y + statusBarHeight + y_Edge,
        editor_Window->rectangle.width - slider_Size - 5.0f,
        editor_Window->rectangle.height - slider_Size - statusBarHeight - y_Edge
    };

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel({window_Slice.x, window_Slice.y, window_Slice.width, 0.0f}, debug_Item->name);

    window_Slice.y += text_Height;
    window_Slice.height -= text_Height;
    DrawType(window_Slice, (char*)debug_Item->entity, debug_Item->type, debug_Item->gui_Entities);

    EndScissorMode();
}

