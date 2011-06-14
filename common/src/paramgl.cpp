/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and 
 * proprietary rights in and to this software and related documentation. 
 * Any use, reproduction, disclosure, or distribution of this software 
 * and related documentation without an express license agreement from
 * NVIDIA Corporation is strictly prohibited.
 *
 * Please refer to the applicable NVIDIA end user license agreement (EULA) 
 * associated with this source code for terms and conditions that govern 
 * your use of this NVIDIA software.
 * 
 */
 
 /*
    ParamListGL
    - class derived from ParamList to do simple OpenGL rendering of a parameter list
    sgg 8/2001
*/
#include "stdafx.h"
#include <param.h>
#include <paramgl.h>
#include <string.h>
#include "SmokeSystem.h"
void beginWinCoords(void)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0, winHeight - 1, 0.0);
    glScalef(1.0, -1.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, winWidth, 0, winHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
}

void endWinCoords(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void glPrint(int x, int y, const char *s, void *font)
{
    glRasterPos2f(x, y);
    int len = (int) strlen(s);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, s[i]);
    }
}

void glPrintShadowed(int x, int y, const char *s, void *font, float *color)
{
    glColor3f(0.0, 0.0, 0.0);
    glPrint(x-1, y-1, s, font);

    glColor3fv((GLfloat *) color);
    glPrint(x, y, s, font);
}


ParamListGL::ParamListGL(const char *name) :
    ParamList(name),
    m_text_color_selected(1.0, 1.0, 1.0),
    m_text_color_unselected(0.75, 0.75, 0.75),
    m_text_color_shadow(0.0, 0.0, 0.0),
    m_bar_color_outer(0.25, 0.25, 0.25),
    m_bar_color_inner(1.0, 1.0, 1.0)
{
    m_font = (void *) GLUT_BITMAP_9_BY_15;
//    m_font = (void *) GLUT_BITMAP_8_BY_13;
    m_font_h = 15;

    m_bar_x = 260;
    m_bar_w = 250;
    m_bar_h = 10;
    m_bar_offset = 5;
    m_text_x = 5;
    m_separation = 15;
    m_value_x = 200;
    m_start_x = 0;
    m_start_y = 0;
}

void
ParamListGL::Render(int x, int y, bool shadow)
{
    beginWinCoords();

    m_start_x = x;
    m_start_y = y;

    for(std::vector<ParamBase *>::const_iterator p = m_params.begin(); p != m_params.end(); ++p) {
        if ((*p)->IsList()) {
            ParamListGL *list = (ParamListGL *) (*p);
            list->Render(x+10, y);
            y += m_separation*list->GetSize();

        } else {
            if (p == m_current)
                glColor3fv(&m_text_color_selected.r);
            else
                glColor3fv(&m_text_color_unselected.r);

            if (shadow) {
                glPrintShadowed(x + m_text_x, y + m_font_h, (*p)->GetName().c_str(), m_font, (p == m_current) ? &m_text_color_selected.r : &m_text_color_unselected.r);
                glPrintShadowed(x + m_value_x, y + m_font_h, (*p)->GetValueString().c_str(), m_font, (p == m_current) ? &m_text_color_selected.r : &m_text_color_unselected.r);
            } else {
                glPrint(x + m_text_x, y + m_font_h, (*p)->GetName().c_str(), m_font);
                glPrint(x + m_value_x, y + m_font_h, (*p)->GetValueString().c_str(), m_font);
            } 

            glColor3fv((GLfloat *) &m_bar_color_outer.r);
            glBegin(GL_LINE_LOOP);
            glVertex2f(x + m_bar_x, y + m_bar_offset);
            glVertex2f(x + m_bar_x + m_bar_w, y + m_bar_offset);
            glVertex2f(x + m_bar_x + m_bar_w, y + m_bar_offset + m_bar_h);
            glVertex2f(x + m_bar_x, y + m_bar_offset + m_bar_h);
            glEnd();

            glColor3fv((GLfloat *) &m_bar_color_inner.r);
            glRectf(x + m_bar_x, y + m_bar_offset + m_bar_h, x + m_bar_x + ((m_bar_w-1)*(*p)->GetPercentage()), y + m_bar_offset + 1);

            y += m_separation;
        }

    }

    endWinCoords();
}


bool
ParamListGL::Mouse(int x, int y, int button, int state)
{
    if ((y < m_start_y) || (y > (int)(m_start_y + (m_separation * m_params.size()) - 1)))
        return false;

    int i = (y - m_start_y) / m_separation;

    if ((button==GLUT_LEFT_BUTTON) && (state==GLUT_DOWN)) {
#if defined(__GNUC__) && (__GNUC__ < 3)
        m_current = &m_params[i];
#else
        // MJH: workaround since the version of vector::at used here is non-standard
        for (m_current = m_params.begin(); m_current != m_params.end() && i > 0; m_current++, i--) ;
        //m_current = (std::vector<ParamBase *>::const_iterator)&m_params.at(i);
#endif

        if ((x > m_bar_x) && (x < m_bar_x + m_bar_w)) {
            Motion(x, y);
        }
    }
    return true;
}

bool
ParamListGL::Motion(int x, int y)
{
    if ((y < m_start_y) || (y > m_start_y + (m_separation * (int)m_params.size()) - 1) )
        return false;

    if (x < m_bar_x||x > m_bar_x + m_bar_w) {
        return false;
    }
    (*m_current)->SetPercentage((x-m_bar_x) / (float) m_bar_w);
    return true;
}

void
ParamListGL::Special(int key, int /*x*/, int /*y*/)
{
    switch(key) {
    case GLUT_KEY_DOWN:
        Increment();
        break;
    case GLUT_KEY_UP:
        Decrement();
        break;
    case GLUT_KEY_RIGHT:
        GetCurrent()->Increment();
        break;
    case GLUT_KEY_LEFT:
        GetCurrent()->Decrement();
        break;
    case GLUT_KEY_HOME:
        GetCurrent()->Reset();
        break;
    case GLUT_KEY_END:
        GetCurrent()->SetPercentage(1.0);
        break;
    }
    //glutPostRedisplay();
}
