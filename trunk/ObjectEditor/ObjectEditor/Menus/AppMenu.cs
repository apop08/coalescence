using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppMenu
    {
        MainMenu _menu;

        public AppMenu()
        {
            _menu = new MainMenu();

            AppFileMenu fileMenu = new AppFileMenu();
            _menu.MenuItems.Add(fileMenu.GetMenuItem());

            AppComponentMenu componentMenu = new AppComponentMenu();
            _menu.MenuItems.Add(componentMenu.GetMenuItem());
        }

        public MainMenu GetMenu()
        {
            return _menu;
        }
    }
}
