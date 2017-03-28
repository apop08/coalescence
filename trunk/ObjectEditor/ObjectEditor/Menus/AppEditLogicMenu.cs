using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppEditLogicMenu
    {
        MenuItem _editLogicItem;

        public AppEditLogicMenu()
        {
            _editLogicItem = new MenuItem("Logic");
            _editLogicItem.Name = "EditLogic";
        }

        public MenuItem GetMenuItem()
        {
            return _editLogicItem;
        }
    }
}