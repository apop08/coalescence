using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppComponentMenu
    {
        MenuItem _componentItem;
        MenuItem _editGraphicsItem = new AppEditGraphicsMenu().GetMenuItem();
        MenuItem _editPhysicsItem = new AppEditPhysicsMenu().GetMenuItem();
        MenuItem _editLogicItem = new AppEditLogicMenu().GetMenuItem();
        MenuItem _editAudioItem = new AppEditAudioMenu().GetMenuItem();
        MenuItem _addComponentItem = new AppAddComponentMenu().GetMenuItem();

        public AppComponentMenu()
        {
            _componentItem = new MenuItem("Components");
            _componentItem.Name = "Components";

            _componentItem.MenuItems.Add(_editGraphicsItem);
            _componentItem.MenuItems[0].Enabled = false;
            _componentItem.MenuItems.Add(_editPhysicsItem);
            _componentItem.MenuItems[1].Enabled = false;
            _componentItem.MenuItems.Add(_editLogicItem);
            _componentItem.MenuItems[2].Enabled = false;
            _componentItem.MenuItems.Add(_editAudioItem);
            _componentItem.MenuItems[3].Enabled = false;

            _componentItem.MenuItems.Add("-");

            _componentItem.MenuItems.Add(_addComponentItem);

            _componentItem.Popup += new EventHandler(_componentItem_Popup);
        }

        void _componentItem_Popup(object sender, EventArgs e)
        {
            for (int i = 0; i < _componentItem.MenuItems.Count; ++i)
            {
                _componentItem.MenuItems[i].Enabled = _componentItem.MenuItems[i].MenuItems.Count > 0 ? true : false;
            }
        }

        public MenuItem GetMenuItem()
        {
            return _componentItem;
        }
    }
}
