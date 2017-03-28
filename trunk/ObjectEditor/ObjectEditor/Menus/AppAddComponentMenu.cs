using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppAddComponentMenu
    {
        MenuItem _addComponentItem;
        MenuItem _addGraphicsItem = new AppAddGraphicsMenu().GetMenuItem();
        MenuItem _addPhysicsItem = new AppAddPhysicsMenu().GetMenuItem();
        MenuItem _addLogicItem = new AppAddLogicMenu().GetMenuItem();
        MenuItem _addAudioItem = new AppAddAudioMenu().GetMenuItem();

        public AppAddComponentMenu()
        {
            _addComponentItem = new MenuItem("Add");
            _addComponentItem.Name = "AddComponent";

            _addComponentItem.MenuItems.Add(_addGraphicsItem);
            _addComponentItem.MenuItems.Add(_addPhysicsItem);
            _addComponentItem.MenuItems.Add(_addLogicItem);
            _addComponentItem.MenuItems.Add(_addAudioItem);
        }

        public MenuItem GetMenuItem()
        {
            return _addComponentItem;
        }
    }
}
