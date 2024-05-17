///
/// System : JPortal
/// $Source: /main/nedcor/cvsroot/dev/jportal/ConnectorOracle.java,v $
/// $Author: vince $
/// $Date: 2002/03/13 11:52:31 $
/// $Revision: 1.2 $
///

package vlab.jportal.util;

import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class ConnectorOracle extends Connector
{
  String userId;
  /**
  ** @param driverType - get inserted into  jdbc:oracle:----:[@server] thin
  ** @param server     - get inserted into  jdbc:oracle:driverType:@----]
  ** @param user
  ** @param password
  */
  public ConnectorOracle(String driverType, String server,
                  String user, String password) throws ClassNotFoundException, SQLException
  {
    String url = "jdbc:oracle:"+driverType+":";
    if (server.length() > 0)
      url = url+"@"+server;
    connect(url, user, password);
  }
  void connect(String url,
               String user, String password) throws ClassNotFoundException, SQLException
  {
    userId = user;
    connection = DriverManager.getConnection(url, user, password);
    System.out.println(!connection.isClosed ());
    connection.setAutoCommit(false);
  }
  public String getUserstamp() throws SQLException
  {
    return userId;
  }
  public int getSequence(String table) throws SQLException
  {
    int nextNo;
    PreparedStatement prep = connection.prepareStatement("select "+table+"Seq.NextVal from dual");
    ResultSet result = prep.executeQuery();
    result.next();
    nextNo =  result.getInt(1);
    result.close();
    prep.close();
    return nextNo;
  }
  public long getBigSequence(String table) throws SQLException
  {
    long nextNo;
    PreparedStatement prep = connection.prepareStatement("select "+table+"Seq.NextVal from dual");
    ResultSet result = prep.executeQuery();
    result.next();
    nextNo =  result.getLong(1);
    result.close();
    prep.close();
    return nextNo;
  }
  public int getSequence(String table, String field) throws SQLException
  {
    return getSequence(table);
  }
  public long getBigSequence(String table, String field) throws SQLException
  {
    return getBigSequence(table);
  }
  public Returning getReturning(String table, String field) throws SQLException
  {
    Returning result = new Returning();
    result.head = "";
    result.output = "";
    result.sequence = table + "Seq.NextVal,"; 
    result.tail = ""; 
    result.dropField = "";
    result.doesGeneratedKeys = false;
    return result;
  }
}

