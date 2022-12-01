using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraTracking : MonoBehaviour
{
    [SerializeField] Transform m_target;
    [SerializeField] Vector3 m_offset;
    Vector3 m_targetForward;
    Vector3 m_newPos;
    // Start is called before the first frame update
    void Start()
    {
        transform.position = m_target.position + m_target.right * m_offset.x + m_target.up * m_offset.y + m_target.forward * m_offset.z;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if(m_target != null)
        {
            m_newPos = m_target.position + m_target.right * m_offset.x + m_target.up * m_offset.y + m_target.forward * m_offset.z;
            transform.position = Vector3.Lerp(transform.position, m_newPos, 0.2f);
            transform.LookAt(m_target);
        }
    }
    public void RemoveTarget()
    {
        m_target = null;
    }
}
