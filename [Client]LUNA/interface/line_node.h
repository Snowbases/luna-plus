// line_node.h: interface for the line_node class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_NODE_H__4B08F8FA_73F2_4C0B_9FA1_4EF1C8FF2F82__INCLUDED_)
#define AFX_LINE_NODE_H__4B08F8FA_73F2_4C0B_9FA1_4EF1C8FF2F82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIN_NODE_TEXT_SIZE	255

class line_node
{
public:
	line_node();
	static void set_line_node(char * text, line_node ** top);
	static void release_line_node(line_node ** top);
	char* curposition;
	size_t len;
	char line[MAX_PATH];
	line_node * nextLine;
	line_node * prevLine;
};

#endif // !defined(AFX_LINE_NODE_H__4B08F8FA_73F2_4C0B_9FA1_4EF1C8FF2F82__INCLUDED_)
