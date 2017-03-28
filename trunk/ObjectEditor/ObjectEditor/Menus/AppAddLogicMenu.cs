using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppAddLogicMenu
    {
        MenuItem _addLogicItem;

        public AppAddLogicMenu()
        {
            _addLogicItem = new MenuItem("Logic");
            _addLogicItem.Name = "AddLogic";

            _addLogicItem.MenuItems.Add("Player");
            //_addLogicItem.MenuItems[0].Click += new EventHandler(AppAddPhysicsMenu_RigidBody_Click);

            _addLogicItem.MenuItems.Add("Bullet");
            //_addLogicItem.MenuItems[0].Click += new EventHandler(AppAddPhysicsMenu_RigidBody_Click);

            _addLogicItem.MenuItems.Add("Block");
            //_addLogicItem.MenuItems[0].Click += new EventHandler(AppAddPhysicsMenu_RigidBody_Click);
        }

        public MenuItem GetMenuItem()
        {
            return _addLogicItem;
        }
    }
}
