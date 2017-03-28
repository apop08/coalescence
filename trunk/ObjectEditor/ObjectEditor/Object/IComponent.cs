using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    public interface IComponent
    {
        string Name
        {
            get;
            set;
        }

        void LoadData();
        void SaveData();
        void UpdateData();

        void Draw(Panel tPanel);
    }
}
