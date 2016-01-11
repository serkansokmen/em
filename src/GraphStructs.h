//
//  GraphStructs.h
//  em
//
//  Created by Serkan Sokmen on 11/01/16.
//
//

#pragma once


namespace gcv {
    
    typedef struct {
        string      id;
        string      name;
        string      description;
        string      image;
        string      color;
    } NodeType;
    
    typedef struct {
        string      id;
        string      name;
        string      description;
        double      weighted;
        double      directed;
        string      color;
    } EdgeType;
    
    typedef struct {
        string      id;
        string      type;
        string      type_id;
        string      name;
        double      pos_x;
        double      pos_y;
    } Node;
    
    typedef struct {
        string      id;
        string      name;
        string      from;
        string      to;
        string      type_id;
        string      user_id;
    } Edge;
    
    typedef struct {
        string      id;
        string      username;
        string      first_name;
        string      last_name;
        string      fullname;
        bool        is_owner;
        bool        is_admin;
        string      img_path;
    } User;
    
};

