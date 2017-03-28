using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    public class SoundEffect : IComponent
    {
        public SoundEffect()
        {
        }

        public SoundEffect(string name)
        {
            _name = name;
        }

        private string _name;
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        public void LoadData()
        {
        }

        public void SaveData()
        {
        }

        public void UpdateData()
        {
        }

        public void Draw(Panel tPanel)
        {
        }
    }
}
