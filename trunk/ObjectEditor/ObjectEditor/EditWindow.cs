using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    public partial class EditWindow : Form
    {
        GameObject _gameObject;
        Panel tempBox;
        Point _offSet = new Point(0, 0);

        public EditWindow()
        {
            InitializeComponent();

            this.Text = "ObjectEditor";

            //This ensures that the form always resizes to fit the image and controls.
            this.AutoSize = true;
            this.AutoSizeMode = AutoSizeMode.GrowAndShrink;

            //Create the menu system.
            AppMenu mainMenu = new AppMenu();
            Menu = mainMenu.GetMenu();

            _gameObject = new GameObject();

            tempBox = new Panel();
            tempBox.Top = 3;
            tempBox.Left = 3;
            tempBox.Height = 400;
            tempBox.Width = 300;
            tempBox.BackColor = Color.Transparent;
            tempBox.Name = "PHPanel";
            tempBox.Paint += new PaintEventHandler(tempBox_Paint);
            this.Controls.Add(tempBox);
        }

        void tempBox_Paint(object sender, PaintEventArgs e)
        {
            for(int i = 0; i < _gameObject._components.Count(); ++i)
            {
                _gameObject._components[i].Draw(tempBox);
            }
        }

        public GameObject GetGameObject()
        {
            return _gameObject;
        }

        public Panel GetPanel()
        {
            return tempBox;
        }

        public Point GetOffset()
        {
            return _offSet;
        }

        public void SetOffset()
        {
            if (_offSet.X != 1 && _offSet.Y != 1)
            {
                _offSet = new Point(1, 1);
                tempBox.Width += 1;
                tempBox.Height += 1;
            }
        }
    }
}
