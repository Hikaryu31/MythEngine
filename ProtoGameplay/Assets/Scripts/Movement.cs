using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{
    [SerializeField] Animator m_animator;
    [SerializeField] float m_runVelocity;
    [SerializeField] float m_dashVelocity;
    [SerializeField] float m_maxDashTime;
    float m_dashTime;
    float m_maxVelocity;
    float m_horizontalAxis;
    float m_verticalalAxis;
    Rigidbody m_rb;
    Vector2 m_oldMousePos;
    Vector2 m_newMousePos;
    Vector3 direction;
    // Start is called before the first frame update
    void Start()
    {
        m_maxVelocity = m_runVelocity;
        m_rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        m_animator.SetBool("Moving", false);
        m_horizontalAxis = Input.GetAxis("Horizontal");
        m_verticalalAxis = Input.GetAxis("Vertical");

        if(m_dashTime <= 0)
        {
            direction.x = -m_verticalalAxis;
            direction.z = m_horizontalAxis;

            if (m_verticalalAxis != 0)
            {
                m_animator.SetBool("Moving", true);
            }
            if (m_horizontalAxis != 0)
            {
                m_animator.SetBool("Moving", true);
            }
        }

        if(Input.GetKeyDown(KeyCode.Space) && m_maxVelocity==m_runVelocity)
        {
            m_maxVelocity = m_dashVelocity;
            m_dashTime = m_maxDashTime;
        }
        m_rb.velocity = (transform.localToWorldMatrix * direction) * m_maxVelocity;

        transform.Rotate(0f, Input.GetAxis("Mouse X"), 0f);
    }
    void LateUpdate()
    {
        if (m_dashTime > 0)
        {
            m_dashTime -= Time.deltaTime;
            if ( m_dashTime < 0.1f)
            {
                m_maxVelocity = m_runVelocity;
            }
        }
    }

    public void Hit()
    {
        if (m_dashTime <= 0)
        {
            Destroy(this.gameObject);
        }
    }
}
