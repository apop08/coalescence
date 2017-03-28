using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppEditGraphicsMenu
    {
        MenuItem _editGraphicsItem;

        public AppEditGraphicsMenu()
        {
            _editGraphicsItem = new MenuItem("Graphics");
            _editGraphicsItem.Name = "EditGraphics";
        }

        public MenuItem GetMenuItem()
        {
            return _editGraphicsItem;
        }
    }
}