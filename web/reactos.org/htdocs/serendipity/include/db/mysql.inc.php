<?php # $Id: mysql.inc.php 565 2005-10-17 13:04:11Z garvinhicking $
# Copyright (c) 2003-2005, Jannis Hermanns (on behalf the Serendipity Developer Team)
# All rights reserved.  See LICENSE file for licensing details

function serendipity_db_begin_transaction(){
    serendipity_db_query('start transaction');
}

function serendipity_db_end_transaction($commit){
    if ($commit){
        serendipity_db_query('commit');
    }else{
        serendipity_db_query('rollback');
    }
}

function serendipity_db_in_sql($col, &$search_ids) {
    return $col . " IN (" . implode(', ', $search_ids) . ")";
}

/* Issues a query to the underlying database;
 * returns:
 *   false if there was an error,
 *   true if the query succeeded but did not generate any rows
 *   array of field values if it returned a single row and $single is true
 *   array of array of field values if it returned row(s)
 */
function &serendipity_db_query($sql, $single = false, $result_type = "both", $reportErr = false, $assocKey = false, $assocVal = false, $expectError = false) {
    global $serendipity;
    static $type_map = array(
                         'assoc' => MYSQL_ASSOC,
                         'num'   => MYSQL_NUM,
                         'both'  => MYSQL_BOTH,
                         'true'  => true,
                         'false' => false
    );

    // highlight_string(var_export($sql, 1));

    if ($expectError) {
        $c = @mysql_query($sql, $serendipity['dbConn']);
    } else {
        $c = mysql_query($sql, $serendipity['dbConn']);
    }

    if (!$expectError && mysql_error($serendipity['dbConn']) != '') {
        $msg = '<pre>' . $sql . '</pre> / ' . mysql_error($serendipity['dbConn']);
        return $msg;
    }

    if (!$c) {
        if (!$expectError && !$serendipity['production']) {
            print '<pre>' . $sql . '</pre> / ' . mysql_error($serendipity['dbConn']);
            if (function_exists('debug_backtrace') && $reportErr == true) {
                highlight_string(var_export(debug_backtrace(), 1));
            }
        }

        return $type_map['false'];
    }
    if ($c === true) {
        return $type_map['true'];
    }

    $result_type = $type_map[$result_type];

    switch(mysql_num_rows($c)) {
        case 0:
            if ($single) {
                return $type_map['false'];
            }
            return $type_map['true'];
        case 1:
            if ($single) {
                return mysql_fetch_array($c, $result_type);
            }
        default:
            if ($single) {
                return mysql_fetch_array($c, $result_type);
            }

            $rows = array();
            while (($row = mysql_fetch_array($c, $result_type))) {
                if (!empty($assocKey) && !empty($assocVal)) {
                    // You can fetch a key-associated array via the two function parameters assocKey and assocVal
                    $rows[$row[$assocKey]] = $row[$assocVal];
                } else {
                    $rows[] = $row;
                }
            }
            return $rows;
    }
}

function serendipity_db_insert_id() {
    global $serendipity;

    return mysql_insert_id($serendipity['dbConn']);
}

function serendipity_db_affected_rows() {
    global $serendipity;

    return mysql_affected_rows($serendipity['dbConn']);
}

function serendipity_db_updated_rows() {
    global $serendipity;

    preg_match(
        "/^[^0-9]+([0-9]+)[^0-9]+([0-9]+)[^0-9]+([0-9]+)/",
        mysql_info(),
        $arr);
        // mysql_affected_rows returns 0 if rows were matched but not changed.
        // mysql_info returns rows matched AND rows changed
        return $arr[2];
}

function serendipity_db_matched_rows() {
    global $serendipity;

    preg_match(
        "/^[^0-9]+([0-9]+)[^0-9]+([0-9]+)[^0-9]+([0-9]+)/",
        mysql_info(),
        $arr);
        // mysql_affected_rows returns 0 if rows were matched but not changed.
        // mysql_info returns rows matched AND rows changed
        return $arr[1];
}

function serendipity_db_escape_string($string) {
    return mysql_escape_string($string);
}

function serendipity_db_limit($start, $offset) {
    return $start . ', ' . $offset;
}

function serendipity_db_limit_sql($limitstring) {
    return ' LIMIT ' . $limitstring;
}

function serendipity_db_connect() {
    global $serendipity;

    if (isset($serendipity['dbConn'])) {
        return $serendipity['dbConn'];
    }

    if (isset($serendipity['dbPersistent']) && $serendipity['dbPersistent']) {
        $function = 'mysql_pconnect';
    } else {
        $function = 'mysql_connect';
    }

    $serendipity['dbConn'] = $function($serendipity['dbHost'], $serendipity['dbUser'], $serendipity['dbPass']);
    mysql_select_db($serendipity['dbName']);
    return $serendipity['dbConn'];
}

function serendipity_db_schema_import($query) {
    static $search  = array('{AUTOINCREMENT}', '{PRIMARY}',
        '{UNSIGNED}', '{FULLTEXT}', '{FULLTEXT_MYSQL}', '{BOOLEAN}');
    static $replace = array('int(11) not null auto_increment', 'primary key',
        'unsigned'  , 'FULLTEXT', 'FULLTEXT', 'enum (\'true\', \'false\') NOT NULL default \'true\''); 
    static $is_utf8 = null;
    global $serendipity;
    
    if ($is_utf8 === null) {
        $search[] = '{UTF_8}'; 
        if (  (isset($_POST['charset']) && $_POST['charset'] == 'UTF-8/') || 
              $serendipity['charset'] == 'UTF-8/' ||
              $serendipity['POST']['charset'] == 'UTF-8/' || 
              LANG_CHARSET == 'UTF-8' ) {
            $replace[] = '/*!40100 CHARACTER SET utf8 COLLATE utf8_unicode_ci */';
        } else {
            $replace[] = '';
        }
    }
    
    $query = trim(str_replace($search, $replace, $query));

    if ($query{0} == '@') {
        // Errors are expected to happen (like duplicate index creation)
        return serendipity_db_query(substr($query, 1), false, 'both', false, false, false, true);
    } else {
        return serendipity_db_query($query);
    }
}

/* probes the usability of the DB during installation */
function serendipity_db_probe($hash, &$errs) {
    global $serendipity;

    if (!function_exists('mysql_connect')) {
        $errs[] = 'No mySQL extension found. Please check your webserver installation or contact your systems administrator regarding this problem.';
        return false;
    }

    if (!($c = @mysql_connect($hash['dbHost'], $hash['dbUser'], $hash['dbPass']))) {
        $errs[] = 'Could not connect to database; check your settings.';
        $errs[] = 'The mySQL error was: ' . mysql_error();
        return false;
    }

    $serendipity['dbConn'] = $c;

    if (!@mysql_select_db($hash['dbName'])) {
        $errs[] = 'The database you specified does not exist.';
        $errs[] = 'The mySQL error was: ' . mysql_error();
        return false;
    }

    return true;
}

function serendipity_db_concat($string) {
    return 'concat(' . $string . ')';
}

/* vim: set sts=4 ts=4 expandtab : */
?>
