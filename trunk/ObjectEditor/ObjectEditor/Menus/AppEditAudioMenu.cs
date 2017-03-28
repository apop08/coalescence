using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppEditAudioMenu
    {
        MenuItem _editAudioItem;

        public AppEditAudioMenu()
        {
            _editAudioItem = new MenuItem("Audio");
            _editAudioItem.Name = "EditAudio";
        }

        public MenuItem GetMenuItem()
        {
            return _editAudioItem;
        }
    }
}