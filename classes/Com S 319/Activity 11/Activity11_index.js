/// Author: Tyler Evans
/// ISU NetID: tevans22@iastate.edu
/// Date: 3/28/23

import React from 'react';
import ReactDOM from 'react-dom/client';
import UserCard from './Activity11_UserCard';

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
    <div>
        <UserCard
            name="Elton John"
            amount={3000}
            married={true}
            points={[100, 101.1, 202, 2]}
            address={{street: "123 Bellmont Rd.", city: "Ames", state: "Iowa"}}
        />
        <UserCard
            name="John Travolta"
            amount={3500}
            married={false}
            points={[1, 2, 3, 4]}
            address={{street: "5010 Av Some", city: "Tempe", state: "AZ"}}
        />
    </div>
);