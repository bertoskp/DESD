export function getSimpleNetworkConfig(j) {
  const jNet = j['network'];

  const states = [];
  const transitions = {};

  jNet['transitions'].forEach(t => {
    const {id, label} = t;
    transitions[id] = {
      label
    };
  });

  jNet['states'].forEach(s => {
    const {id} = s;
    states.push({
      id,
      starting: s.hasOwnProperty('starting'),
      final: s.hasOwnProperty('final')
    });
    if (s.hasOwnProperty('in') && s['in'] !== null) s['in'].forEach(tIn => transitions[tIn].target = id);
    if (s.hasOwnProperty('out') && s['out'] !== null) s['out'].forEach(tOut => transitions[tOut].source = id)
  })

  const elements = states.map(s => {
    return { data: s }
  });

  Object.keys(transitions).forEach(k => elements.push({
    data: transitions[k]
  }));

  return {
    elements: elements,
    style: [ // the stylesheet for the graph
      {
        selector: 'node',
        style: {
          'background-color': el => {
            if (el._private.data.starting) return '#4444CC';
            if (el._private.data.final) return '#44CC44';
            return '#666666'
          },
          'label': 'data(id)',
          'text-valign': 'center'
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
          'label': 'data(label)'
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