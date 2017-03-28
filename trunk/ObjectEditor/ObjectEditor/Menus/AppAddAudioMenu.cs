using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppAddAudioMenu
    {
        MenuItem _addAudioItem;

        public AppAddAudioMenu()
        {
            _addAudioItem = new MenuItem("Audio");
            _addAudioItem.Name = "AddAudio";

            _addAudioItem.MenuItems.Add("Sound Effect");
            _addAudioItem.MenuItems[0].Click += new EventHandler(AppAddAudioMenu_SoundEffect_Click);
        }

        void AppAddAudioMenu_SoundEffect_Click(object sender, EventArgs e)
        {
            GameObject tempObj = ((EditWindow)EditWindow.ActiveForm).GetGameObject();
            SoundEffect tempSfx = new SoundEffect("New Sound Effect");
            tempObj.AddComponent(tempSfx);

            MenuItem tempItem = _addAudioItem.GetMainMenu().MenuItems["Components"].MenuItems["EditAudio"];
            tempItem.MenuItems.Add(tempSfx.Name);

            ((EditWindow)EditWindow.ActiveForm).Controls.RemoveByKey("PropertyTool");

            SfxPropertyTool sTool = new SfxPropertyTool(tempSfx);
            sTool.Top = 0;
            sTool.Left = ((EditWindow)EditWindow.ActiveForm).Controls["PHPanel"].Right + 5;
            sTool.Name = "PropertyTool";
            ((EditWindow)EditWindow.ActiveForm).Controls.Add(sTool);
        }

        public MenuItem GetMenuItem()
        {
            return _addAudioItem;
        }
    }
}