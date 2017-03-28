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
    public partial class SfxPropertyTool : PropertyTools
    {
        public SfxPropertyTool()
        {
            InitializeComponent();

            _pNameLabel.Text = "Edit Sound Effect:";
        }

        public SfxPropertyTool(SoundEffect tempSfx)
        {
            InitializeComponent();

            _pNameLabel.Text = "Edit Sound Effect:";
            _pNameBox.Text = tempSfx.Name;
        }
    }
}
