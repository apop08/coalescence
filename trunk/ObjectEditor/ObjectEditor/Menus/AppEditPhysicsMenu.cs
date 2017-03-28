using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppEditPhysicsMenu
    {
        MenuItem _editPhysicsItem;

        public AppEditPhysicsMenu()
        {
            _editPhysicsItem = new MenuItem("Physics");
            _editPhysicsItem.Name = "EditPhysics";
        }

        public MenuItem GetMenuItem()
        {
            return _editPhysicsItem;
        }
    }
}