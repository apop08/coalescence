using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppAddGraphicsMenu
    {
        MenuItem _addGraphicsItem;

        public AppAddGraphicsMenu()
        {
            _addGraphicsItem = new MenuItem("Graphics");
            _addGraphicsItem.Name = "AddGraphics";

            _addGraphicsItem.MenuItems.Add("Sprite");
            _addGraphicsItem.MenuItems[0].Click += new EventHandler(AppAddGraphicsMenu_Sprite_Click);

            _addGraphicsItem.MenuItems.Add("Animated Sprite");

            _addGraphicsItem.MenuItems.Add("Composite Sprite");
        }

        void AppAddGraphicsMenu_Sprite_Click(object sender, EventArgs e)
        {
            OpenFileDialog openSprite = new OpenFileDialog();
            openSprite.Filter = "PNG Image Files (*.png)|*.png";
            openSprite.FileOk += new System.ComponentModel.CancelEventHandler(Sprite_Add_FileOk);
            openSprite.ShowDialog();
        }

        void Sprite_Add_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Sprite tempSprite = new Sprite(((OpenFileDialog)sender).FileName);
            GameObject tempObj = ((EditWindow)(Application.OpenForms[0])).GetGameObject();

            ((EditWindow)(Application.OpenForms[0])).Controls.RemoveByKey("PropertyTool");

            SpritePropertyTool sTool = new SpritePropertyTool(tempSprite);
            sTool.Top = 0;
            sTool.Left = ((EditWindow)(Application.OpenForms[0])).Controls["PHPanel"].Right + 5;
            sTool.Name = "PropertyTool";
            ((EditWindow)(Application.OpenForms[0])).Controls.Add(sTool);

            tempObj.AddComponent(tempSprite);
            MenuItem tempItem = _addGraphicsItem.GetMainMenu().MenuItems["Components"].MenuItems["EditGraphics"];
            tempItem.MenuItems.Add(tempSprite.Name);
            tempItem.MenuItems[tempItem.MenuItems.Count - 1].Click += new EventHandler(AppAddGraphicsMenu_Sprite_Edit_Click);
        }

        void AppAddGraphicsMenu_Sprite_Edit_Click(object sender, EventArgs e)
        {
            GameObject tempObj = ((EditWindow)(Application.OpenForms[0])).GetGameObject();
            Sprite tempSprite = (Sprite)tempObj.GetComponentByName(((MenuItem)sender).Text);

            ((EditWindow)(Application.OpenForms[0])).Controls.RemoveByKey("PropertyTool");

            SpritePropertyTool sTool = new SpritePropertyTool(tempSprite);
            sTool.Top = 0;
            sTool.Left = ((EditWindow)(Application.OpenForms[0])).Controls["PHPanel"].Right + 5;
            sTool.Name = "PropertyTool";
            ((EditWindow)(Application.OpenForms[0])).Controls.Add(sTool);
        }

        public MenuItem GetMenuItem()
        {
            return _addGraphicsItem;
        }
    }
}