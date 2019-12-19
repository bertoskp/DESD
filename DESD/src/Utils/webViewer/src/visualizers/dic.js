
export function displayDictionary(j) {
    const jDic = j['dictionary'];

    const transitions = {};
    jDic['transitions'].forEach(t => {
        const label = t['id'] + ': ' + t['label'];
        transitions[t['id']] = {id: label};
    });

    const states = [];
    jDic['states'].forEach(s => {
        let label = s['id'] + ': ';

        if (s.hasOwnProperty('diagnosis') && s['diagnosis'] !== null) label += s['diagnosis'].map(sd => '{' + sd.join(',') + '}');

        label = ' ' + label + ' ';

        if (s.hasOwnProperty('in') && s['in'] !== null) s['in'].forEach(tId => {
            transitions[tId].target = label;
        });

        if (s.hasOwnProperty('out') && s['out'] !== null) s['out'].forEach(tId => {
            transitions[tId].source = label;
        });


        states.push({
            id: label,
            starting: s.hasOwnProperty('starting'),
            final: s.hasOwnProperty('final')
        })
    });

    const elements = [];
    states.forEach(s => elements.push({data: s}));
    Object.keys(transitions).forEach(k => elements.push({data: transitions[k]}));

    return {
        container: document.getElementById('graph'),
        elements: elements,

        style: [ // the stylesheet for the graph
            {
                selector: 'node',
                style: {
                'background-color': el =>
                el._private.data.starting ? '#1ad000' :
                    (el._private.data.final ? '#9667ee' :
                        (el._private.data.noOutputs ? '#b39435' : '#acabb0')),
                    
                    'label': 'data(id)',
                    'text-valign': 'center',
                    'shape': 'rectangle',
                    'width': 'label'
                }
            },

            {
                selector: 'edge',
                style: {
                    'width': 2,
                    'line-color': '#ccc',
                    'target-arrow-color': '#ccc',
                    'curve-style': 'bezier',
                    'target-arrow-shape': 'triangle',
                    'target-endpoint': 'outside-to-node',
                    'label': 'data(id)'
                }
            }
        ],

        layout: {
            name: 'cose',
            fit: true,
            componentSpacing: 120,
        }
    };
}
