/// Author: Tyler Evans
/// ISU NetID: tevans22@iastate.edu
/// Date: 3/28/23

import React from 'react';

export default function UserCard({
    name,
    amount,
    married,
    points,
    address
}) {
    return (
        <div>
            <h1>{name}</h1>
            <p>$ {amount}</p>
            <p>{married ? 'married' : 'single'}</p>
            <ul>
                <li>Street: {address.street}</li>
                <li>City: {address.city}</li>
                <li>State: {address.state}</li>
            </ul>
        </div>
    );
}