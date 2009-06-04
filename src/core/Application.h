#pragma once

#include <QApplication>

namespace Jerboa
{
	class Plugin;
	class Application: public QApplication
	{
		Q_OBJECT;
		public:
			Application(int& argc, char** argv);
		private slots:
			void loadFirstRunWizard();
			void loadMainWindow();
		private:
			void setupDatabase();

			QList<Plugin*> m_plugins;
	};
};
