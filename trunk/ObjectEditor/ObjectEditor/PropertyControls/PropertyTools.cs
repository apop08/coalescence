using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    public partial class PropertyTools : UserControl
    {
        protected Label _pSeparatorLine;
        protected Label _pNameLabel;
        protected TextBox _pNameBox;

        public PropertyTools()
        {
            InitializeComponent();

            //This isn't doing anything since these controls don't change in size.
            //More than anything it was a lazy way to size the control properly.
            this.AutoSize = true;
            this.AutoSizeMode = AutoSizeMode.GrowAndShrink;

            _pSeparatorLine = new Label();
            _pSeparatorLine.Top = 3;
            _pSeparatorLine.Left = 0;
            _pSeparatorLine.Height = Application.OpenForms[0].Controls["PHPanel"].Height;
            _pSeparatorLine.Width = 2;
            _pSeparatorLine.BorderStyle = BorderStyle.Fixed3D;
            this.Controls.Add(_pSeparatorLine);

            _pNameLabel = new Label();
            _pNameLabel.Top = 0;
            _pNameLabel.Left = _pSeparatorLine.Right + 2;
            _pNameLabel.Height = 15;
            _pNameLabel.Text = "Component Name:";
            this.Controls.Add(_pNameLabel);

            _pNameBox = new TextBox();
            _pNameBox.Top = _pNameLabel.Bottom;
            _pNameBox.Left = _pNameLabel.Left + 5;
            _pNameBox.Width = 195;
            this.Controls.Add(_pNameBox);
        }
    }
}
