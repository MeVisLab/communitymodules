<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <!-- import global MeVis stylesheet -->
  <xsl:import href="$${MLAB_MeVis_BuildSystem_URL}/BuildTools/DocTools/Files/xsl/docbook/mevis/html.xsl"/>

  <!-- Add your own customization here. To see what can be customized, \
       refer to the global MeVis stylesheet above \
       Most likely you want to customize the page footer here. \
    -->

  <!-- remove tabel of figures for book -->
  <xsl:param name="generate.toc">
      appendix  toc,title
      article/appendix  nop
      article   toc,title
      book      toc,title
      chapter   toc,title
      part      toc,title
      preface   toc,title
      qandadiv  toc
      qandaset  toc
      reference toc,title
      sect1     toc
      sect2     toc
      sect3     toc
      sect4     toc
      sect5     toc
      section   toc
      set       toc,title
  </xsl:param>

</xsl:stylesheet>
