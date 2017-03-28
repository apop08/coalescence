using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppAddPhysicsMenu
    {
        MenuItem _addPhysicsItem;

        public AppAddPhysicsMenu()
        {
            _addPhysicsItem = new MenuItem("Physics");
            _addPhysicsItem.Name = "AddPhysics";

            _addPhysicsItem.MenuItems.Add("Rigid Body");
            _addPhysicsItem.MenuItems[0].Click += new EventHandler(AppAddPhysicsMenu_RigidBody_Click);

            _addPhysicsItem.MenuItems.Add("Region");
        }

        void AppAddPhysicsMenu_RigidBody_Click(object sender, EventArgs e)
        {
            RigidBody tempRB = new RigidBody("New Rigid Body");
            GameObject tempObj = ((EditWindow)(Application.OpenForms[0])).GetGameObject();

            ((EditWindow)Application.OpenForms[0]).Controls.RemoveByKey("PropertyTool");

            RigidBodyPropertyTool rTool = new RigidBodyPropertyTool(tempRB);
            rTool.Top = 0;
            rTool.Left = ((EditWindow)Application.OpenForms[0]).Controls["PHPanel"].Right + 5;
            rTool.Name = "PropertyTool";
            ((EditWindow)Application.OpenForms[0]).Controls.Add(rTool);

            tempObj.AddComponent(tempRB);
            MenuItem tempItem = _addPhysicsItem.GetMainMenu().MenuItems["Components"].MenuItems["EditPhysics"];
            tempItem.MenuItems.Add(tempRB.Name);
        }

        public MenuItem GetMenuItem()
        {
            return _addPhysicsItem;
        }
    }
}