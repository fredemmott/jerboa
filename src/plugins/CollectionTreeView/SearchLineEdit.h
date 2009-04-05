/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _SEARCH_LINE_EDIT_H
#define _SEARCH_LINE_EDIT_H

#include <QLineEdit>

/** Widget for search terms.
 * Modelled after OS X.
 * @author Fred Emmott <mail@fredemmott.co.uk>
 */
class SearchLineEdit : public QLineEdit
{
	Q_OBJECT
	public:
		SearchLineEdit(QWidget* parent = NULL);
	protected:
		/** Draw extra parts of the search editor.
		 * - Magnifying glass
		 * - Clear button
		 * - "Search" light grey text if empty and unfocused
		 */
		void paintEvent(QPaintEvent* event);

		/** Change the cursor depending on the position.
		 * Only show the I beam if we're not over the clear button
		 * or the magnifying glass.
		 */
		void mouseMoveEvent(QMouseEvent* event);
		/// Handle pressing the clear button.
		void mouseReleaseEvent(QMouseEvent* event);
	private:
		/// How much horizontal space the magnifying glass takes up.
		int m_leftPadding;
		/// The rectangle occupied by the clear button.
		QRectF m_clearButton;
};

#endif
