using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ObjectEditor
{
    public class GameObject
    {
        public List<IComponent> _components = new List<IComponent>();

        public GameObject()
        {
        }

        public void AddComponent(IComponent tempComp)
        {
            _components.Add(tempComp);
        }

        public IComponent GetComponentByName(string name)
        {
            return _components.Find(item => item.Name == name);
        }

        public override string ToString()
        {
            string tempString = null;

            foreach (IComponent comp in _components)
            {
                tempString += comp;
                tempString += Environment.NewLine;
            }

            return tempString;
        }
    }
}
