using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    class AppFileMenu
    {
        MenuItem _fileItem;
        string _curFile = null;

        public AppFileMenu()
        {
            _fileItem = new MenuItem("File");

            _fileItem.MenuItems.Add("New");
            _fileItem.MenuItems[0].Click += new EventHandler(AppFileMenu_New_Click);
            _fileItem.MenuItems.Add("Open");
            _fileItem.MenuItems[1].Click += new EventHandler(AppFileMenu_Open_Click);
            _fileItem.MenuItems.Add("Save");
            _fileItem.MenuItems[2].Click += new EventHandler(AppFileMenu_Save_Click);
            _fileItem.MenuItems.Add("Quit");
            _fileItem.MenuItems[3].Click += new EventHandler(AppFileMenu_Quit_Click);
        }

        void AppFileMenu_Quit_Click(object sender, EventArgs e)
        {
            Application.OpenForms[0].Close();
        }

        void AppFileMenu_Save_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFile = new SaveFileDialog();
            saveFile.Filter = "Game Object Files (*.sobj)|*.sobj";
            saveFile.FileOk += new System.ComponentModel.CancelEventHandler(FileSave_FileOk);
            saveFile.ShowDialog();
        }

        void FileSave_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _curFile = ((SaveFileDialog)sender).FileName;
            GameObject tempObj = ((EditWindow)(Application.OpenForms[0])).GetGameObject();

            Application.OpenForms[0].Text = "ObjectEditor - Current File: " + _curFile;

            System.IO.File.WriteAllText(_curFile, tempObj.ToString());
        }

        void AppFileMenu_Open_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "Game Object Files (*.sobj)|*.sobj";
            openFile.FileOk += new System.ComponentModel.CancelEventHandler(FileOpen_FileOk);
            openFile.ShowDialog();
        }

        void FileOpen_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _curFile = ((OpenFileDialog)sender).FileName;

            Application.OpenForms[0].Text = "ObjectEditor - Current File: " + _curFile;
        }

        void AppFileMenu_New_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "Game Object Files (*.sobj)|*.sobj";
            openFile.FileOk += new System.ComponentModel.CancelEventHandler(FileNew_FileOk);
            openFile.ShowDialog();
        }

        void FileNew_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {
        }

        public MenuItem GetMenuItem()
        {
            return _fileItem;
        }
    }
}
