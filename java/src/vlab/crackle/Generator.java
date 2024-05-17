/// ------------------------------------------------------------------
/// Copyright (c) from 1996 Vincent Risi 
///
/// All rights reserved. 
/// This program and the accompanying materials are made available 
/// under the terms of the Common Public License v1.0 
/// which accompanies this distribution and is available at 
/// http://www.eclipse.org/legal/cpl-v10.html 
/// Contributors:
///    Vincent Risi
/// ------------------------------------------------------------------

package vlab.crackle;

import java.io.FileInputStream;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.Properties;
import java.util.Vector;

public abstract class Generator
{
  protected static Vector<Pragma> pragmaVector;
  private static Properties moduleProperties;

  {
    pragmaVector = new Vector<Pragma>();
  }

  public static void generate(Module module, String output, PrintWriter outLog)
  {
  }

  public static String description()
  {
    return "This description should be implemented in the generator";
  }

  public static String documentation()
  {
    return "This documentation should be implemented in the generator";
  }

  public static Vector<Pragma> pragma()
  {
    return pragmaVector;
  }

  public static void loadProperties(Module module, String path)
  {
    try
    {
      String propertiesName = String.format("%s%s.properties", path, module.name);
      InputStream input = new FileInputStream(propertiesName);
      moduleProperties = new Properties();
      moduleProperties.load(input);
    }
    catch (Exception ex)
    {
      moduleProperties = null;
    }
  }

  public static String getProperty(String propName, String propDefault)
  {
    if (moduleProperties == null)
      return propDefault;
    String propValue = moduleProperties.getProperty(propName);
    if (propValue == null)
      return propDefault;
    return propValue;
  }

  public static class Pragma
  {
    public String name;
    public boolean value;
    public String description;

    public Pragma(String name, boolean value, String description)
    {
      this.name = name;
      this.value = value;
      this.description = description;
    }
  }
}
